#include <esp_wifi.h>
#include <esp_event.h>
#include <esp_log.h>
#include <esp_system.h>
#include <nvs_flash.h>
#include <sys/param.h>
#include "nvs_flash.h"
#include "esp_netif.h"
#include "esp_eth.h"
// #include "protocol_examples_common.h"
#include "ota.h"
#include "esp_http_server.h"
#include "webServer.h"
#include "wifiStation.h"

/* A simple example that demonstrates how to create GET and POST
 * handlers for the web server.
 */

static const char *TAG = "webServer";

/* The examples use WiFi configuration that you can set via project configuration menu.

   If you'd rather not, just change the below entries to strings with
   the config you want - ie #define EXAMPLE_WIFI_SSID "mywifissid"
*/
#define EXAMPLE_ESP_WIFI_SSID      "valon2.4G"
#define EXAMPLE_ESP_WIFI_PASS      "cmpf42ea"
#define EXAMPLE_ESP_WIFI_CHANNEL   0
#define EXAMPLE_MAX_STA_CONN       2

static void wifi_event_handler(void* arg, esp_event_base_t event_base,
                                    int32_t event_id, void* event_data)
{
    if (event_id == WIFI_EVENT_AP_STACONNECTED) {
        wifi_event_ap_staconnected_t* event = (wifi_event_ap_staconnected_t*) event_data;
        ESP_LOGI(TAG, "station "MACSTR" join, AID=%d",
                 MAC2STR(event->mac), event->aid);
    } else if (event_id == WIFI_EVENT_AP_STADISCONNECTED) {
        wifi_event_ap_stadisconnected_t* event = (wifi_event_ap_stadisconnected_t*) event_data;
        ESP_LOGI(TAG, "station "MACSTR" leave, AID=%d",
                 MAC2STR(event->mac), event->aid);
    }
}

void wifi_init_softap(void)
{
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_create_default_wifi_ap();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT,
                                                        ESP_EVENT_ANY_ID,
                                                        &wifi_event_handler,
                                                        NULL,
                                                        NULL));

    wifi_config_t wifi_config = {
        .ap = {
            .ssid = EXAMPLE_ESP_WIFI_SSID,
            .ssid_len = strlen(EXAMPLE_ESP_WIFI_SSID),
            .channel = EXAMPLE_ESP_WIFI_CHANNEL,
            .password = EXAMPLE_ESP_WIFI_PASS,
            .max_connection = EXAMPLE_MAX_STA_CONN,
            .authmode = WIFI_AUTH_WPA_WPA2_PSK
        },
    };
    if (strlen(EXAMPLE_ESP_WIFI_PASS) == 0) {
        wifi_config.ap.authmode = WIFI_AUTH_OPEN;
    }

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_AP, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());

    ESP_LOGI(TAG, "wifi_init_softap finished. SSID:%s password:%s channel:%d",
             EXAMPLE_ESP_WIFI_SSID, EXAMPLE_ESP_WIFI_PASS, EXAMPLE_ESP_WIFI_CHANNEL);
}

/* An HTTP GET handler */
static esp_err_t hello_get_handler(httpd_req_t *req)
{
    char*  buf;
    size_t buf_len;

    httpd_resp_set_type(req, "text/html");

    /* Get header value string length and allocate memory for length + 1,
     * extra byte for null termination */
    buf_len = httpd_req_get_hdr_value_len(req, "Host") + 1;
    if (buf_len > 1) {
        buf = malloc(buf_len);
        /* Copy null terminated value string into buffer */
        if (httpd_req_get_hdr_value_str(req, "Host", buf, buf_len) == ESP_OK) {
            ESP_LOGI(TAG, "Found header => Host: %s", buf);
        }
        free(buf);
    }

    buf_len = httpd_req_get_hdr_value_len(req, "Test-Header-2") + 1;
    if (buf_len > 1) {
        buf = malloc(buf_len);
        if (httpd_req_get_hdr_value_str(req, "Test-Header-2", buf, buf_len) == ESP_OK) {
            ESP_LOGI(TAG, "Found header => Test-Header-2: %s", buf);
        }
        free(buf);
    }

    buf_len = httpd_req_get_hdr_value_len(req, "Test-Header-1") + 1;
    if (buf_len > 1) {
        buf = malloc(buf_len);
        if (httpd_req_get_hdr_value_str(req, "Test-Header-1", buf, buf_len) == ESP_OK) {
            ESP_LOGI(TAG, "Found header => Test-Header-1: %s", buf);
        }
        free(buf);
    }

    /* Read URL query string length and allocate memory for length + 1,
     * extra byte for null termination */
    buf_len = httpd_req_get_url_query_len(req) + 1;
    if (buf_len > 1) {
        buf = malloc(buf_len);
        if (httpd_req_get_url_query_str(req, buf, buf_len) == ESP_OK) {
            ESP_LOGI(TAG, "Found URL query => %s", buf);
            char param[32];
            /* Get value of expected key from query string */
            if (httpd_query_key_value(buf, "query1", param, sizeof(param)) == ESP_OK) {
                ESP_LOGI(TAG, "Found URL query parameter => query1=%s", param);
            }
            if (httpd_query_key_value(buf, "query3", param, sizeof(param)) == ESP_OK) {
                ESP_LOGI(TAG, "Found URL query parameter => query3=%s", param);
            }
            if (httpd_query_key_value(buf, "query2", param, sizeof(param)) == ESP_OK) {
                ESP_LOGI(TAG, "Found URL query parameter => query2=%s", param);
            }
        }
        free(buf);
    }

    /* Set some custom headers */
    // httpd_resp_set_hdr(req, "Custom-Header-1", "Custom-Value-1");
    // httpd_resp_set_hdr(req, "Custom-Header-2", "Custom-Value-2");

    /* Send response with custom headers and body set as the
     * string passed in user context*/
    const char* resp_str = (const char*) req->user_ctx;
    httpd_resp_send(req, resp_str, strlen(resp_str));

    /* After sending the HTTP response the old HTTP request
     * headers are lost. Check if HTTP request headers can be read now. */
    if (httpd_req_get_hdr_value_len(req, "Host") == 0) {
        ESP_LOGI(TAG, "Request headers lost");
    }
    return ESP_OK;
}

static const httpd_uri_t home_url = {
    .uri       = "/",
    .method    = HTTP_GET,
    .handler   = hello_get_handler,
    /* Let's pass response string in user
     * context to demonstrate it's usage */
    .user_ctx  = "<!DOCTYPE html>\n<html>\n    <style>\n        html {\n            width: 100%;\n            height: 100%;\n            margin: 0;\n            padding: 0;\n        }\n\n        body {\n            width: 100%;\n            height: 100%;\n            margin: 0;\n            padding: 0;\n            background-color:rgba(211, 211, 211, 0.295);\n            overflow-x: hidden;\n            text-align: center;\n        }\n\n        div.loadBotton {\n            background-color: rgba(100, 148, 237, 0.432);\n            font-size: 1.5rem;\n            margin: auto;\n            padding-left: 1rem;\n            padding-right: 1rem;\n        }\n        div.loadBotton:hover {\n            background-color: cornflowerblue;\n        }\n    </style>\n\n    <script>\n        var maxReqLen = 1024 * 10\n\n        function fileUpload(file) {\n            reader = new FileReader();\n\n            reader.onload = evt => {\n                let remainingSize = evt.target.result.byteLength\n                let blob = new Blob([evt.target.result])\n                let offset = 0\n\n                startBlob = new Blob([\"start\", remainingSize])\n                console.log(startBlob)\n\n                doneBlob = new Blob([\"done\", remainingSize])\n                console.log(doneBlob)\n\n                xhr = new XMLHttpRequest()\n                xhr.open('put', '/upgradeCntl', false);\n                xhr.setRequestHeader('Content-Type', 'text/plain')\n                xhr.send(startBlob)\n\n                while (remainingSize > 0) {\n                    data = blob.slice(offset, offset + maxReqLen)\n\n                    xhr = new XMLHttpRequest()\n                    xhr.open('post', '/post', false);\n                    xhr.setRequestHeader('Content-Type', 'text/plain')\n                    xhr.send(data)\n\n                    offset += maxReqLen\n                    remainingSize -= maxReqLen\n\n                    console.log('offset:' + offset)\n                    console.log('remainingSize' + remainingSize)\n                }\n\n                xhr = new XMLHttpRequest()\n                xhr.open('put', '/upgradeCntl', false);\n                xhr.setRequestHeader('Content-Type', 'text/plain')\n                xhr.send(doneBlob)\n            }\n\n            reader.readAsArrayBuffer(file);\n        }\n\n        function upload()\n        {\n            file = document.getElementById(\"fileUpLoad\").files[0];\n            fileUpload(file);\n        }\n    </script>\n\n    <body>\n        <input type=\"file\"  id=\"fileUpLoad\" onchange=\"upload()\">\n    </body>\n</html>"
};

#define OTA_REQ_RECV_BUFFER_SIZE        1024
/* An HTTP POST handler */
static esp_err_t echo_post_handler(httpd_req_t *req)
{
    char *buf;
    int ret, remaining = req->content_len;

    buf = malloc(OTA_REQ_RECV_BUFFER_SIZE);

    // ESP_LOGI(TAG, "=========== RECEIVED DATA ==========");
    while (remaining > 0) {
        /* Read the data for the request */
        ret = httpd_req_recv(req, buf,
                        MIN(remaining, OTA_REQ_RECV_BUFFER_SIZE));
        if (ret <= 0) {
            if (ret == HTTPD_SOCK_ERR_TIMEOUT) {
                /* Retry receiving if timeout occurred */
                continue;
            }

            ota_upgradeAbort();
            free(buf);
            return ESP_FAIL;
        }

        /* Send back the same data */
        // httpd_resp_send_chunk(req, buf, ret);
        remaining -= ret;

        ota_recvData((uint8_t *)buf, ret);
        
        /* Log data received */
        // ESP_LOGI(TAG, "recv data len:%d", ret);
    }
    // ESP_LOGI(TAG, "====================================");

    // End response
    httpd_resp_send_chunk(req, NULL, 0);

    free(buf);
    return ESP_OK;
}

static const httpd_uri_t firmwarePost = {
    .uri       = "/post",
    .method    = HTTP_POST,
    .handler   = echo_post_handler,
    .user_ctx  = NULL
};

/* This handler allows the custom error handling functionality to be
 * tested from client side. For that, when a PUT request 0 is sent to
 * URI /ctrl, the /hello and /echo URIs are unregistered and following
 * custom error handler http_404_error_handler() is registered.
 * Afterwards, when /hello or /echo is requested, this custom error
 * handler is invoked which, after sending an error message to client,
 * either closes the underlying socket (when requested URI is /echo)
 * or keeps it open (when requested URI is /hello). This allows the
 * client to infer if the custom error handler is functioning as expected
 * by observing the socket state.
 */
esp_err_t http_404_error_handler(httpd_req_t *req, httpd_err_code_t err)
{
    if (strcmp("/hello", req->uri) == 0) {
        httpd_resp_send_err(req, HTTPD_404_NOT_FOUND, "/hello URI is not available");
        /* Return ESP_OK to keep underlying socket open */
        return ESP_OK;
    } else if (strcmp("/echo", req->uri) == 0) {
        httpd_resp_send_err(req, HTTPD_404_NOT_FOUND, "/echo URI is not available");
        /* Return ESP_FAIL to close underlying socket */
        return ESP_FAIL;
    }
    /* For any other URI send 404 and close socket */
    httpd_resp_send_err(req, HTTPD_404_NOT_FOUND, "Some 404 error message");
    return ESP_FAIL;
}

/* An HTTP PUT handler. This demonstrates realtime
 * registration and deregistration of URI handlers
 */
static esp_err_t ctrl_put_handler(httpd_req_t *req)
{
    char buf[20];
    int ret;
    bool reboot;

    if ((ret = httpd_req_recv(req, &buf, sizeof(buf))) <= 0) {
        if (ret == HTTPD_SOCK_ERR_TIMEOUT) {
            httpd_resp_send_408(req);
        }
        return ESP_FAIL;
    }

    ESP_LOGI(TAG, "Upgrade cntl: cmd %s", buf);

    reboot = false;
    if (memcmp(buf, "start", 5) == 0) {
        ESP_LOGI(TAG, "Upgrade cntl: start");
        
        ota_upgradeStart(atoi(&buf[5]));
    } else if (memcmp(buf, "done", 4) == 0) {
        ESP_LOGI(TAG, "Upgrade cntl: done");

        if (!ota_checkImage()) {
            reboot = true;
        }
    }

    /* Respond with empty body */
    httpd_resp_send(req, NULL, 0);

    if (reboot) {
        ota_reboot();
    }

    return ESP_OK;
}

static const httpd_uri_t ctrl = {
    .uri       = "/upgradeCntl",
    .method    = HTTP_PUT,
    .handler   = ctrl_put_handler,
    .user_ctx  = NULL
};

static httpd_handle_t start_webserver(void)
{
    httpd_handle_t server = NULL;
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    config.lru_purge_enable = true;

    // Start the httpd server
    ESP_LOGI(TAG, "Starting server on port: '%d'", config.server_port);
    if (httpd_start(&server, &config) == ESP_OK) {
        // Set URI handlers
        ESP_LOGI(TAG, "Registering URI handlers");
        httpd_register_uri_handler(server, &home_url);
        httpd_register_uri_handler(server, &firmwarePost);
        httpd_register_uri_handler(server, &ctrl);
        return server;
    }

    ESP_LOGI(TAG, "Error starting server!");
    return NULL;
}

static void stop_webserver(httpd_handle_t server)
{
    // Stop the httpd server
    httpd_stop(server);
}

static void disconnect_handler(void* arg, esp_event_base_t event_base, 
                               int32_t event_id, void* event_data)
{
    httpd_handle_t* server = (httpd_handle_t*) arg;
    if (*server) {
        ESP_LOGI(TAG, "Stopping webserver");
        stop_webserver(*server);
        *server = NULL;
    }
}

static void connect_handler(void* arg, esp_event_base_t event_base, 
                            int32_t event_id, void* event_data)
{
    httpd_handle_t* server = (httpd_handle_t*) arg;
    if (*server == NULL) {
        ESP_LOGI(TAG, "Starting webserver");
        *server = start_webserver();
    }
}


void 
webServer_task_entry(void *arg)
{
    static httpd_handle_t server = NULL;

    wifiStation_connect();

    /* Ensure to disable any WiFi power save mode, this allows best throughput
     * and hence timings for overall OTA operation.
     */
    // esp_wifi_set_ps(WIFI_PS_NONE);

    /* Start the server for the first time */
    server = start_webserver();
}