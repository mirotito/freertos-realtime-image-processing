// main.c  (C, not C++)
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#include "FreeRTOS.h"
#include "task.h"
#include "image_bridge.h"

#define IMG_DIR "D:/embeded after understanding logic/images/"
#define STACK_SZ 256
#define PIPELINE_BUDGET_MS 100

static const int idx = 0;

/* Paths */
char cap_path[260], gray_path[260], blur_path[260], edge_path[260], invert_path[260];
char current_image[260];

/* Timing */
TickType_t t_gray_ms, t_blur_ms, t_edge_ms, t_invert_exec_ms;
int invert_delay_ms;

/* Sync */
volatile int finished_tasks = 0;

/* Task handles */
TaskHandle_t hCapture, hGray, hBlur, hEdge, hInvert;

/* ----------------- Helpers ----------------- */
void task_done(void)
{
    finished_tasks++;
    if (finished_tasks == 4)
    {
        TickType_t total =
            t_gray_ms + t_blur_ms + t_edge_ms + t_invert_exec_ms;

        printf("\n--------------------------------------------\n");
        printf("Gray   : %lu ms\n", t_gray_ms);
        printf("Blur   : %lu ms\n", t_blur_ms);
        printf("Edge   : %lu ms\n", t_edge_ms);
        printf("Invert : %lu ms (exec) + %d ms (delay)\n",
            t_invert_exec_ms, invert_delay_ms);
        printf("TOTAL PIPELINE TIME: %lu ms\n", total);

        if (total > PIPELINE_BUDGET_MS)
            printf("⚠️  WARNING: Pipeline exceeded budget!\n");
        else
            printf("✅ Pipeline within 100 ms budget.\n");

        printf("--------------------------------------------\n");
    }
}

/* ----------------- Tasks ----------------- */

void Capture_Task(void* pv)
{
    cam_capture_crop_and_save(cap_path, 100, 100, 200, 200);
    strcpy(current_image, cap_path);

    xTaskNotifyGive(hGray);
    xTaskNotifyGive(hBlur);
    xTaskNotifyGive(hEdge);
    xTaskNotifyGive(hInvert);

    vTaskDelete(NULL);
}

void Gray_Task(void* pv)
{
    ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
    TickType_t s = xTaskGetTickCount();

    img_grayscale_process(current_image, gray_path);
    strcpy(current_image, gray_path);

    t_gray_ms = (xTaskGetTickCount() - s) * portTICK_PERIOD_MS;
    task_done();
    vTaskDelete(NULL);
}

void Blur_Task(void* pv)
{
    ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
    TickType_t s = xTaskGetTickCount();

    img_blur_process(current_image, blur_path);
    strcpy(current_image, blur_path);

    t_blur_ms = (xTaskGetTickCount() - s) * portTICK_PERIOD_MS;
    task_done();
    vTaskDelete(NULL);
}

void Edge_Task(void* pv)
{
    ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
    TickType_t s = xTaskGetTickCount();

    img_edge_process(current_image, edge_path);
    strcpy(current_image, edge_path);

    t_edge_ms = (xTaskGetTickCount() - s) * portTICK_PERIOD_MS;
    task_done();
    vTaskDelete(NULL);
}

void Invert_Task(void* pv)
{
    ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

    /* BLOCK EARLY */
    if (invert_delay_ms > 0)
        vTaskDelay(pdMS_TO_TICKS(invert_delay_ms));

    TickType_t s = xTaskGetTickCount();

    img_invert_process(current_image, invert_path);
    strcpy(current_image, invert_path);

    t_invert_exec_ms = (xTaskGetTickCount() - s) * portTICK_PERIOD_MS;
    task_done();
    vTaskDelete(NULL);
}

/* ----------------- Main ----------------- */
int main(void)
{
    int pGray, pBlur, pEdge, pInvert;

    srand((unsigned int)time(NULL));
    invert_delay_ms = rand() % 90;

    printf("Invert random delay decided early = %d ms\n", invert_delay_ms);

    printf("Enter priorities (1–4):\n");
    printf("Gray   : "); scanf("%d", &pGray);//4
    printf("Blur   : "); scanf("%d", &pBlur);//3
    printf("Edge   : "); scanf("%d", &pEdge);//2
    printf("Invert : "); scanf("%d", &pInvert);//1

    /* Priority adjustment */
    if (invert_delay_ms > 50)      pInvert = 4;
    else if (invert_delay_ms > 35) pInvert = 3;
    else if (invert_delay_ms > 25) pInvert = 2;

    printf("\nFinal priorities:\n");
    printf("Gray   = %d\n", pGray);
    printf("Blur   = %d\n", pBlur);
    printf("Edge   = %d\n", pEdge);
    printf("Invert = %d\n\n", pInvert);

    snprintf(cap_path, 260, IMG_DIR "capture_%03d.jpg", idx);
    snprintf(gray_path, 260, IMG_DIR "gray_%03d.jpg", idx);
    snprintf(blur_path, 260, IMG_DIR "blur_%03d.jpg", idx);
    snprintf(edge_path, 260, IMG_DIR "edge_%03d.jpg", idx);
    snprintf(invert_path, 260, IMG_DIR "invert_%03d.jpg", idx);

    xTaskCreate(Capture_Task, "Capture", STACK_SZ, NULL, 5, &hCapture);
    xTaskCreate(Gray_Task, "Gray", STACK_SZ, NULL, pGray, &hGray);
    xTaskCreate(Blur_Task, "Blur", STACK_SZ, NULL, pBlur, &hBlur);
    xTaskCreate(Edge_Task, "Edge", STACK_SZ, NULL, pEdge, &hEdge);
    xTaskCreate(Invert_Task, "Invert", STACK_SZ, NULL, pInvert, &hInvert);

    vTaskStartScheduler();
    for (;;);
}
