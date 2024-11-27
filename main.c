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
        printf("Invalid input. Just put a filename");
        return 1;
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

    outFile = fopen(outFilename, "w+");
    
    if(outFile == NULL){
        fprintf(stderr, "Unable to open output file \"%s\".\n", outFilename);
    }

    fclose(outFile);

    // Okay. More I/O stuff... like what do I even do now?

    // Set up the context

    int err = 0;

    AVFormatContext *formatCtx = NULL;
    // I'll come back; I really don't know what this all does

    if ((err = avformat_open_input(&formatCtx, filename, NULL, 0))){
        return printError("Error opening file.", err);
    }

    // I believe that now we need to find the stream... at some point I should 
    // start putting some of these things in helper functions

    


return 0;

}


