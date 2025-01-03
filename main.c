# include <stdio.h>
# include <stdlib.h>
# include <libavcodec/avcodec.h>
# include <libavformat/avformat.h>
# include <libavutil/imgutils.h>

FILE* outFile;

int printError(const char * prefix, int errorCode){
    if (!errorCode){
        return 0;
    } else {
        const size_t bufsize = 64;
        char buf[bufsize];

        if (av_strerror(errorCode, buf, bufsize)){
            strcpy(buf, "WHAT THE FUCK");
        }

        fprintf(stderr, "%s (%d: %s)\n", prefix, errorCode, buf);

        return errorCode;
    }
}

int main(int argc, char *argv[]){
    // present I/O functionality is that you can input the filename

    // check to make sure that there are only "2" args

    if (argc != 2){
        printf("Invalid input. Just put a filename\n");
        exit(1);
    }

    // get the filename
    char* filename = argv[1];

    // open the outfile called "<infile>.raw"

    char outFilename[strlen(filename) + 5];
    // yeah this works fine there was no reason to allocate space like that
  
    strcpy(outFilename, filename);
    strcpy(outFilename + strlen(filename), ".raw");
    // ^^ think about how that works. Remember pointer arithmetic?
    // this is also already incredibly unsafe code lmao

    // open the outfile to make sure it works

    outFile = fopen(outFilename, "w+");
    
    if(outFile == NULL){
        fprintf(stderr, "Unable to open output file \"%s\".\n", outFilename);
    }

    fclose(outFile);

    // Okay, now we set up our AVFormatContext

    // Set up the context

    int err = 0;

    AVFormatContext *formatCtx = NULL;
    // declare our AVFormatContext

    if ((err = avformat_open_input(&formatCtx, filename, NULL, 0))){
        // returns 0 if it works, error is returned otherwise
        // this is why we use printError
        return printError("Error opening file.", err);
    }

    // formatCtx struct field nb_stream is integer, has number of streams
    int no_strms = formatCtx->nb_streams;

    // now, we must iterate htrough the streams and find the video stream!
    AVStream **strms = formatCtx->streams;
    

    int vid_strm_idx = -1;

    for(int idx = 0; idx < no_strms; idx++){
        if(strms[idx]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO){
            vid_strm_idx = idx;
            break;
        }
    }

    if (vid_strm_idx == -1){
        // if we're here, there was no video stream. this shouldn't happen.
        printf("this really shouldn't happen... video stream not found\n");
        exit(1);
    }

    // store stream locally
    AVStream * vid_strm = strms[vid_strm_idx];

    // store codec params locally, codec params contained in AVStream *vid_strm
    AVCodecParameters * codecpar = vid_strm->codecpar;
    int codec_id = codecpar->codec_id;

    printf("codec_id: %d\n", codec_id);

    // initialize decoder
    const AVCodec * decoder = avcodec_find_decoder(codec_id);

    // normal stuff making sure that worked

    if(!decoder){
        printf("this shouldn't happen; no decoder was found for the thing\n");
        exit(1);
    }

    // now it looks like we need to set up what's called a decodercontext

    AVCodecContext * codec_context = avcodec_alloc_context3(decoder);
    
    if(!codec_context){
        printf("this also shouldn't happen; no context could be created for codec\n");
        exit(1);
    }

    // now, according to perplexity, we need to get the relevant codec params
    // into our new context


    if((err = avcodec_parameters_to_context(codec_context, codecpar))){
        return printError("error copying parameters into codec context\n", err);
    }

    // as your days so your life
    // now we open the codec

    if((err = avcodec_open2(codec_context, decoder, NULL))){
        return printError("error opening the codec\n", err);
    }

    // I swear I will put this all in helper functions eventually. 
    // First thing, allocate a packet pointer and a frame pointer

    AVPacket * pk_ptr = av_packet_alloc();
    AVFrame * frm = av_frame_alloc();


    // now, fetch a packet from our context.
    // My understanding is, read a frame from the context (packet)
    // decode packet 

    // av_read_frame returns 0 when successful, negative error codes for EOF

    int frm_count = 0;

    // av_read_frame demuxes and gets us a packet
    // 0 if good, then various errors otherwise. Need to handle them

    int out;

    while(1){
        out = av_read_frame(formatCtx, pk_ptr);

        if (out){
            // non-zero rv, either EOF or something bad!
            if (out == AVERROR_EOF){
                printf("end of file!\n");
                break;
            }
            
            return printError("av_read_frame error", out);
        }

        // if we're here, we got a normal rv, and so pk_ptr now contains smth

        // need to check if it's a video stream

        if (pk_ptr->stream_index != vid_strm_idx){
            // if here, it was not a video stream packet
            continue;
        }

        // ** DECODING **

        if ((out = avcodec_send_packet(codec_context, pk_ptr))){
            return printError("avcodec_send_packet error", out);
        }

        // I really don't know why I need to flush here; if I don't, 
        // it doesn't work

        if ((out = avcodec_send_packet(codec_context, NULL))){
            return printError("avcodec_send_packet NULL Flush error", out);
        }

        // receiving

        while(1){
            out = avcodec_receive_frame(codec_context, frm);
            if (out) {

                if (out == AVERROR_EOF){
                    av_frame_unref(frm);
                    avcodec_flush_buffers(codec_context);
                    break;
                }

                printf("%d\n", out);
                return printError("receive_frame error", out);
            }
            frm_count++;
            av_frame_unref(frm);
        }
    }

    // while (!av_read_frame(formatCtx, pk_ptr)) {
    //     if (pk_ptr->stream_index != vid_strm_idx){
    //         // printf("wrong idx on packet\n");
    //         continue;
    //     }

    //     // otherwise, we're dealing with a vid ptr
    //     // now, we decode

    //     if ((err = avcodec_send_packet(codec_context, pk_ptr))){
    //         printf("there was an error in decoding\n");
    //         continue;
    //     }

    //     // could be multiple frames, now we receive. due to multiple frames, while loop

    //     while ((err = avcodec_receive_frame(codec_context, frm)) >= 0){
    //         // printf("we have a frame!\n");
    //         frm_count++;
    //         av_frame_unref(frm);
    //     }

    //     printf("error code: %d\n", err);

    //     // printError("why did we stop?", err);
    // }

    printf("total number of video frames viewed: %d\n", frm_count);

    return 0;

}


