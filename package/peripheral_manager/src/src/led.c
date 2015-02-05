#include <syslog.h>
#include "log.h"
#include "led.h"

static struct blob_buf bblob;

typedef enum {
    LED_OFF,
    LED_OK,
    LED_NOTICE,
    LED_ALERT,
    LED_ERROR,
    LED_CUSTOM,
    LED_ACTION_MAX,
} led_action_t;

typedef enum {
    LEDS_NORMAL,
    LEDS_PROXIMITY,
    LEDS_SILENT,
    LEDS_INFO,
    LEDS_TEST,
    LEDS_PROD,
    LEDS_RESET,
    LEDS_ALLON,
    LEDS_ALLOFF,
    LEDS_MAX,
} leds_state_t;

/* Names for led_action_t */
static const char * const fn_actions[LED_ACTION_MAX] =
{ "off", "ok", "notice", "alert", "error", "custom"};

#define LED_FUNCTIONS 14
static const char* const led_functions[LED_FUNCTIONS] =
{ "dsl", "wifi", "wps", "lan", "status", "dect", "tv", "usb",
  "wan", "internet", "voice1", "voice2", "eco", "gbe"};

/* Names for led_state_t */
static const char* const led_states[LED_STATES_MAX] =
{ "off", "on", "flash_slow", "flash_fast","breading", "fadeon", "fadeoff" };

/* Names for leds_state_t */
static const char* const leds_states[LEDS_MAX] =
{ "normal", "proximity", "silent", "info", "test", "production", "reset", "allon" , "alloff"};

#define MAX_LEDS (LED_FUNCTIONS)


/* lowest level, contain states, timers,pointer to driver for a single physical led.*/
struct led {
	struct list_head list;
	led_state_t state;		/* state that this led should have, set from the config file */
	struct led_drv *drv;
};

/*middle layer contains lists of leds /buttons/... that should be set to a specific state */
struct function_action {
	const char *name;		/* If name is set this led action is in use by the board. */
	struct list_head led_list;
	struct list_head button_list;
};

/* main struct for the function leds.*/
struct function_led {
	const char *name;		/* If name is set this led function is in use by the board.        */
	led_action_t state;		/* state of the function led. contain what action is currently set */
	struct function_action actions[LED_ACTION_MAX];
};

struct function_led leds[LED_FUNCTIONS];


int get_index_by_name(const char *const*array, int max, const char *name);
struct led_drv *get_drv_led(char *name);
void dump_drv_list(void);
void dump_led(void);


/* we find out the index for a match in an array of char pointers containing max number of pointers */
int get_index_by_name(const char *const*array, int max, const char *name)
{
	int i;
	for (i=0; i < max ; i++ ){
		if (!strcasecmp(name, array[i]))
			return i;
	}
	return 0;
}

/* PUT every led from drivers into a list */
struct drv_led_list{
	struct list_head list;
	struct led_drv *drv;
};
LIST_HEAD(drv_leds_list);

void led_add( struct led_drv *drv)
{
	struct drv_led_list *drv_node = malloc(sizeof(struct drv_led_list));

	DBG(1,"called with led name [%s]\n", drv->name);
	drv_node->drv = drv;

	list_add(&drv_node->list, &drv_leds_list);
}

/* go over the driver list for any led name that matches name and returna pointer to driver. */
struct led_drv *get_drv_led(char *name)
{
	struct list_head *i;
	list_for_each(i, &drv_leds_list) {
		struct drv_led_list *node = list_entry(i, struct drv_led_list, list);
		if (!strcmp(node->drv->name, name))
			return node->drv;
	}
	return NULL;
}

void dump_drv_list(void)
{
	struct list_head *i;
	list_for_each(i, &drv_leds_list) {
		struct drv_led_list *node = list_entry(i, struct drv_led_list, list);
		DBG(1,"led name = [%s]\n",node->drv->name);
	}
}

void dump_led(void)
{
	int i,j;

	for (i = 0; i < LED_FUNCTIONS ; i++) {
		for (j = 0 ; j < LED_ACTION_MAX; j++ ) {
			if ( leds[i].actions[j].name != NULL ) {
				struct led *led;
				list_for_each_entry(led, &leds[i].actions[j].led_list, list) {
					DBG(2,"%-15s %-8s %-15s %-10s\n",
					    leds[i].name,
					    leds[i].actions[j].name,
					    led->drv->name,
					    led_states[led->state]);
}}}}}

enum {
	LED_STATE,
	__LED_MAX
};

static const struct blobmsg_policy led_policy[] = {
	[LED_STATE] = { .name = "state", .type = BLOBMSG_TYPE_STRING },
};

static int led_set_method(struct ubus_context *ubus_ctx, struct ubus_object *obj,
                          struct ubus_request_data *req, const char *method,
                          struct blob_attr *msg)
{
	DBG(1,"led_set_method (%s)\n",method);

	return 0;
}

static int led_status_method(struct ubus_context *ubus_ctx, struct ubus_object *obj,
                             struct ubus_request_data *req, const char *method,
                             struct blob_attr *msg)
{
	DBG(1,"\n");
	blob_buf_init (&bblob, 0);
//    blobmsg_add_string(&bblob, "state", fn_actions[action]);
	blobmsg_add_string(&bblob, "state", "led_fixme_state");
	ubus_send_reply(ubus_ctx, req, bblob.head);

	return 0;
}

static int leds_set_method(struct ubus_context *ubus_ctx, struct ubus_object *obj,
                           struct ubus_request_data *req, const char *method,
                           struct blob_attr *msg)
{
	DBG(1,"\n");

	return 0;
}

static int leds_status_method(struct ubus_context *ubus_ctx, struct ubus_object *obj,
                              struct ubus_request_data *req, const char *method,
                              struct blob_attr *msg)
{
	DBG(1,"\n");

	blob_buf_init (&bblob, 0);
//	blobmsg_add_string(&bblob, "state", leds_states[led_cfg->leds_state]);
	blobmsg_add_string(&bblob, "state", "leds_fixme_state");
	ubus_send_reply(ubus_ctx, req, bblob.head);
	return 0;
}

static const struct ubus_method led_methods[] = {
	UBUS_METHOD("set", led_set_method, led_policy),
	{ .name = "status", .handler = led_status_method },
};

static struct ubus_object_type led_object_type =
	UBUS_OBJECT_TYPE("led", led_methods);



static const struct ubus_method leds_methods[] = {
	UBUS_METHOD("set", leds_set_method, led_policy),
	{ .name = "status", .handler = leds_status_method },
//    { .name = "proximity", .handler = leds_proximity_method },
};

static struct ubus_object_type leds_object_type =
	UBUS_OBJECT_TYPE("leds", leds_methods);


#define LED_OBJECTS 15
static struct ubus_object led_objects[LED_OBJECTS] = {
    { .name = "leds",	        .type = &leds_object_type, .methods = leds_methods, .n_methods = ARRAY_SIZE(leds_methods), },
    { .name = "led.dsl",	.type = &led_object_type, .methods = led_methods, .n_methods = ARRAY_SIZE(led_methods), },
    { .name = "led.wifi",	.type = &led_object_type, .methods = led_methods, .n_methods = ARRAY_SIZE(led_methods), },
    { .name = "led.wps",	.type = &led_object_type, .methods = led_methods, .n_methods = ARRAY_SIZE(led_methods), },
    { .name = "led.lan",	.type = &led_object_type, .methods = led_methods, .n_methods = ARRAY_SIZE(led_methods), },
    { .name = "led.status",	.type = &led_object_type, .methods = led_methods, .n_methods = ARRAY_SIZE(led_methods), },
    { .name = "led.dect",	.type = &led_object_type, .methods = led_methods, .n_methods = ARRAY_SIZE(led_methods), },
    { .name = "led.tv",		.type = &led_object_type, .methods = led_methods, .n_methods = ARRAY_SIZE(led_methods), },
    { .name = "led.usb",	.type = &led_object_type, .methods = led_methods, .n_methods = ARRAY_SIZE(led_methods), },
    { .name = "led.wan",	.type = &led_object_type, .methods = led_methods, .n_methods = ARRAY_SIZE(led_methods), },
    { .name = "led.internet",	.type = &led_object_type, .methods = led_methods, .n_methods = ARRAY_SIZE(led_methods), },
    { .name = "led.voice1",	.type = &led_object_type, .methods = led_methods, .n_methods = ARRAY_SIZE(led_methods), },
    { .name = "led.voice2",	.type = &led_object_type, .methods = led_methods, .n_methods = ARRAY_SIZE(led_methods), },
    { .name = "led.eco",	.type = &led_object_type, .methods = led_methods, .n_methods = ARRAY_SIZE(led_methods), },
    { .name = "led.gbe",	.type = &led_object_type, .methods = led_methods, .n_methods = ARRAY_SIZE(led_methods), },
};


void led_init( struct server_ctx *s_ctx)
{
	int i,j,ret;
//	char *led_names;
	DBG(1,"\n");

	/* map */

	dump_drv_list();
	/* register leds with ubus */

	for (i=0 ; i<LED_OBJECTS ; i++) {
		ret = ubus_add_object(s_ctx->ubus_ctx, &led_objects[i]);
		if (ret)
			DBG(1,"Failed to add object: %s\n", ubus_strerror(ret));
	}

	/* we create a top list of led functions */
	/* that list contains a new list of actions */
	/* every action contains led actions lists */
	/* the led states is attached to the drv_leds lists */

//	led_names = ucix_get_option(s_ctx->uci_ctx, "hw", "board", "lednames");

	for (i = 0; i < LED_FUNCTIONS ; i++) {
		for (j = 0 ; j < LED_ACTION_MAX; j++ ) {
			char fn_name_action[256];

			LIST_HEAD(led_action_list);
			struct ucilist *node;

			snprintf(fn_name_action, 256, "%s_%s", led_functions[i], fn_actions[j]);
			ucix_get_option_list( s_ctx->uci_ctx, "hw", fn_name_action, "led_action" , &led_action_list);

			INIT_LIST_HEAD( &leds[i].actions[j].led_list );

			if (!list_empty(&led_action_list)) {

				/* Found led with action, init structs */
				leds[i].name  = led_functions[i];
				leds[i].state = LED_OFF;

				leds[i].actions[j].name    = fn_actions[j];

				/* fill in led actions */
				DBG(1,"%-15s has led actions -> \n",fn_name_action);
				list_for_each_entry(node, &led_action_list, list) {
					char led_name[256],led_state[256];
					struct led *led;
					struct led_drv *drv;

					/* get pointer to low level led driver.*/
					sscanf(node->val, "%[^=]=%s", led_name, led_state);
					drv = get_drv_led(led_name);

					if (drv) {
						led = malloc(sizeof(struct led));
						led->drv = drv;
						led->state = get_index_by_name(led_states, LED_STATES_MAX, led_state);
						list_add(&led->list, &leds[i].actions[j].led_list);
					}else {
						syslog(LOG_ERR,"Config specified use of led name [%s]. But it's not registerd with a led driver.\n", led_name);
					}
					DBG(1, "%-35s%s\n","",node->val);
				}

				/* fill in button actions */

				/* fill in xxx actions */
			}
		}
	}
	dump_led();
}

