#include "esp_log.h"
#include "esp_event.h"
#include "esp_console.h"
#include <unistd.h>
#include "consola.h"

#include "mock-flash.h"

ESP_EVENT_DEFINE_BASE(ANIOT_CONSOLE_EVENT);

static esp_event_loop_handle_t loop_console;

// Default values
esp_console_repl_config_t repl_config = ESP_CONSOLE_REPL_CONFIG_DEFAULT();
esp_console_dev_uart_config_t hw_config = ESP_CONSOLE_DEV_UART_CONFIG_DEFAULT();

esp_console_repl_t *repl = NULL;

// Postea un evento de la familia ANIOT_CONSOLE_EVENT e ID CMD_QUOTA_EVENT.
static int run_quota()
{

    ssize_t ocupado = getDataLeft();
    ESP_LOGI("", "Ocupados: %d bytes", ocupado);
    return 0;
}

// Postea un evento de la familia ANIOT_CONSOLE_EVENT e ID CMD_MONITOR_EVENT.
static int run_monitor()
{

    // Se postea el evento
    ESP_ERROR_CHECK(esp_event_post_to(loop_console, ANIOT_CONSOLE_EVENT, CMD_MONITOR_EVENT, NULL, 0, portMAX_DELAY));
    return 0;
}

/* Inicia la consola. Requiere que se le pase el event loop donde posteará los eventos
   asociados a cada uno de los comandos.*/
esp_event_loop_handle_t aniot_console_init()
{

    esp_event_loop_args_t loop_console_args = {
        .queue_size = 5,
        .task_name = "console_task",
        .task_priority = uxTaskPriorityGet(NULL),
        .task_stack_size = 3072,
        .task_core_id = tskNO_AFFINITY};

    ESP_ERROR_CHECK(esp_event_loop_create(&loop_console_args, &loop_console));

    return loop_console;
}

void aniot_console_start()
{

    /* Drain stdout before reconfiguring it */
    fflush(stdout);
    fsync(fileno(stdout));

    /* Disable buffering on stdin */
    setvbuf(stdin, NULL, _IONBF, 0);

    const esp_console_cmd_t monitor_cmd = {
        .command = "monitor",
        .help = "Volver a modo de monitorización.",
        .hint = NULL,
        .func = run_monitor,
        .argtable = NULL};

    const esp_console_cmd_t quota_cmd = {
        .command = "quota",
        .help = "Consultar cuantos bytes hay ocupados en la flash.",
        .hint = NULL,
        .func = run_quota,
        .argtable = NULL};

    if (repl != NULL)
    {
        repl->del(repl);
        repl = NULL;
    }

    // Registramos los comandos.
    esp_console_register_help_command();
    ESP_ERROR_CHECK(esp_console_cmd_register(&monitor_cmd));
    ESP_ERROR_CHECK(esp_console_cmd_register(&quota_cmd));

    /* Prompt to be printed before each line.
     * This can be customized, made dynamic, etc.
     */
    repl_config.prompt = "esp32>";
    repl_config.max_cmdline_length = CONFIG_CONSOLE_MAX_COMMAND_LINE_LENGTH;

    ESP_ERROR_CHECK(esp_console_new_repl_uart(&hw_config, &repl_config, &repl));
    ESP_ERROR_CHECK(esp_console_start_repl(repl));
}

void aniot_console_stop()
{

    if (repl != NULL)
    {
        repl->del(repl);
        repl = NULL;
    }
}