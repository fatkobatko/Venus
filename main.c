// src/main.c

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <mosquitto.h>
#include <stdio.h>
#include <string.h>

static bool mqtt_connected = false;

void on_connect(struct mosquitto *mosq, void *userdata, int rc)
{
    if(rc == 0){
        mqtt_connected = true;
        printf("MQTT connected\n");
    } 
    else 
    {
        fprintf(stderr, "MQTT connect failed: %d\n", rc);
    }
}

int main(int argc, char *argv[])
{
    // check if init worked
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        fprintf(stderr, "SDL_Init: %s\n", SDL_GetError());
        return 1;
    }

    if (TTF_Init() != 0) {
        fprintf(stderr, "TTF_Init: %s\n", TTF_GetError());
        SDL_Quit();
        return 1;
    }
    //connecting window

    SDL_Window *win = SDL_CreateWindow
    (
        "future MQTT map rendering app",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        640, 640,        
        0
    );

    
    SDL_Renderer *ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED);

    TTF_Font *font = TTF_OpenFont
    (
        "C:/Windows/Fonts/arial.ttf",  // adjust path if needed
        28
    );
    
    if (!font) {
        fprintf(stderr, "TTF_OpenFont: %s\n", TTF_GetError());
        TTF_Quit();
        SDL_DestroyWindow(win);
        SDL_Quit();
        return 1;
    }

    // create "surface" for the text
    SDL_Color white = { 255, 255, 255, 255 };
    SDL_Surface *surf = TTF_RenderText_Blended(font, "Connecting . . .", white);
    SDL_Texture *text_tex = SDL_CreateTextureFromSurface(ren, surf);
    int texW = surf->w, texH = surf->h;
    SDL_FreeSurface(surf);

    // center the text
    SDL_Rect dst = 
    {
        .x = (640 - texW) / 2,
        .y = (540 - texH) / 2,
        .w = texW,
        .h = texH
    };

    SDL_SetRenderDrawColor(ren, 20,20,20,255);
    SDL_RenderClear(ren);
    SDL_RenderCopy(ren, text_tex, NULL, &dst);
    SDL_RenderPresent(ren);

    //async init
    mosquitto_lib_init();
    struct mosquitto *mosq = mosquitto_new(NULL, true, win);

    mosquitto_connect_callback_set(mosq, on_connect);
    mosquitto_loop_start(mosq);

    //async connecting to serwer
    if (mosquitto_connect_async(mosq, "mqtt.ics.ele.tue.nl", 1883, 60)) 
    {
        fprintf(stderr, "Failed to connect to broker\n");
        return 1;
    }


    // waiting for fct on_connect to flip "mqtt_connected"

    SDL_Event e;
    while (!mqtt_connected) 
    {
        while (SDL_PollEvent(&e)) 
        {
            if (e.type == SDL_QUIT)
                goto clean;
        }
        SDL_Delay(50);
    }

    SDL_SetWindowTitle(win, "MQTT Connected! Starting App…");
    SDL_Delay(500);
 
    SDL_SetWindowTitle(win, "Mapping App");
    SDL_SetWindowSize(win, 800, 600);

    //proper part of the app. SDL MQTT and mapping programm

clean:
    mosquitto_loop_stop(mosq, true);
    mosquitto_destroy(mosq);
    mosquitto_lib_cleanup();

    SDL_DestroyWindow(win);
    SDL_Quit();
    return 0;
} 
