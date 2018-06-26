/** @file Video.c
 * @ingroup   Video
 * @defgroup  Video
 * @brief     Video subsystem.
 * @author    Michael Fitzmayer
 * @copyright "THE BEER-WARE LICENCE" (Revision 42)
 */

#include <stdio.h>
#include <stdlib.h>
#include "Video.h"

/**
 * @brief   Initialise video subsystem.
 * @param   pcTitle      the title of the window, in UTF-8 encoding.
 * @param   s32Width     the width of the window, in screen coordinates.
 * @param   s32Height    the height of the window, in screen coordinates.
 * @param   u8Fullscreen the window's fullscreen state.
 * @param   dZoomLevel   the zoom level used by the renderer.
 * @return  A Video structure or NULL on failure.
 *          See @ref struct Video.
 * @ingroup Video
 */
Video *InitVideo(
    const char    *pcTitle,
    const int32_t  s32Width,
    const int32_t  s32Height,
    const uint8_t  u8Fullscreen,
    const double   dZoomLevel)
{
    static Video *pstVideo;
    pstVideo = malloc(sizeof(struct Video_t));

    if (NULL == pstVideo)
    {
        fprintf(stderr, "InitVideo(): error allocating memory.\n");
        return NULL;
    }

    if (0 != SDL_Init(SDL_INIT_VIDEO))
    {
        fprintf(stderr, "%s\n", SDL_GetError());
        free(pstVideo);
        return NULL;
    }

    pstVideo->s32WindowHeight  = s32Height;
    pstVideo->s32WindowWidth   = s32Width;
    pstVideo->dZoomLevel       = dZoomLevel;
    pstVideo->dZoomLevelInital = dZoomLevel;

    uint32_t u32Flags;
    if (u8Fullscreen)
    {
        u32Flags = SDL_WINDOW_FULLSCREEN_DESKTOP;
    }
    else
    {
        u32Flags = 0;
    }

    pstVideo->pstWindow = SDL_CreateWindow(
        pcTitle,
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        pstVideo->s32WindowWidth,
        pstVideo->s32WindowHeight,
        u32Flags);

    if (NULL == pstVideo->pstWindow)
    {
        fprintf(stderr, "%s\n", SDL_GetError());
        free(pstVideo);
        return NULL;
    }

    if (u8Fullscreen)
    {
        SDL_GetWindowSize(pstVideo->pstWindow, &pstVideo->s32WindowWidth, &pstVideo->s32WindowHeight);
        if (0 > SDL_ShowCursor(SDL_DISABLE))
        {
            fprintf(stderr, "%s\n", SDL_GetError());
            free(pstVideo);
            return NULL;
        }
    }

    pstVideo->pstRenderer = SDL_CreateRenderer(
        pstVideo->pstWindow,
        -1,
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE);

    if (NULL == pstVideo->pstRenderer)
    {
        fprintf(stderr, "%s\n", SDL_GetError());
        free(pstVideo);
        return NULL;
    }

    if (0 != SDL_RenderSetLogicalSize(
            pstVideo->pstRenderer,
            pstVideo->s32WindowWidth  / dZoomLevel,
            pstVideo->s32WindowHeight / dZoomLevel))
    {
        fprintf(stderr, "%s\n", SDL_GetError());
        free(pstVideo);
        return NULL;
    }

    return pstVideo;
}

/**
 * @brief   Set the renderer's zoom level.
 * @param   pstVideo   A Video structure.  See @ref struct Video.
 * @param   dZoomLevel the zoom level
 * @ingroup Video
 * @return  0 on success, -1 on failure.
 */
int8_t SetVideoZoomLevel(Video *pstVideo, double dZoomLevel)
{
    if (dZoomLevel < 1) dZoomLevel = 1;

    if (0 != SDL_RenderSetLogicalSize(
            pstVideo->pstRenderer,
            pstVideo->s32WindowWidth  / dZoomLevel,
            pstVideo->s32WindowHeight / dZoomLevel))
    {
        fprintf(stderr, "%s\n", SDL_GetError());
        return -1;
    }

    pstVideo->dZoomLevel = dZoomLevel;

    return 0;
}

/**
 * @brief   Terminate video subsystem.
 * @param   pstVideo a Video structure.  See @ref struct Video.
 * @ingroup Video
 */
void TerminateVideo(Video *pstVideo)
{
    if ((NULL == pstVideo->pstWindow))
    {
        fprintf(stderr, "%s\n", SDL_GetError());
    }

    SDL_DestroyRenderer(pstVideo->pstRenderer);
    SDL_DestroyWindow(pstVideo->pstWindow);
    free(pstVideo);
}

void UpdateVideo(SDL_Renderer *pstRenderer)
{
    SDL_RenderPresent(pstRenderer);
    SDL_RenderClear(pstRenderer);
}