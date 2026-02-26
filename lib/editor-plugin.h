/** \file editor-plugin.h
 *  \brief Header: editor plugin API for mcedit extensions
 */

#ifndef MC__EDITOR_PLUGIN_H
#define MC__EDITOR_PLUGIN_H

#include "lib/global.h"

/*** typedefs(not structures) and defined constants **********************************************/

#define MC_EDITOR_PLUGIN_API_VERSION 2
#define MC_EDITOR_PLUGIN_ENTRY       "mc_editor_plugin_register"
#define MC_EDITOR_PLUGIN_CMD_BASE    30000L
#ifndef MC_EDITOR_PLUGINS_DIR
#define MC_EDITOR_PLUGINS_DIR "/usr/lib/mc/editor-plugins"
#endif

/*** enums ***************************************************************************************/

typedef enum
{
    MC_EPR_OK = 0,
    MC_EPR_FAILED = -1,
    MC_EPR_NOT_SUPPORTED = -2
} mc_ep_result_t;

typedef enum
{
    MC_EPF_NONE = 0,
    MC_EPF_HAS_MENU = 1 << 0
} mc_ep_flags_t;

typedef struct mc_ep_state_t
{
    gboolean available;
    gboolean enabled;
    const char *reason;
} mc_ep_state_t;

/*** structures declarations (and typedefs of structures)*****************************************/

/* What mcedit provides to a plugin */
typedef struct mc_editor_host_t
{
    void (*refresh) (struct mc_editor_host_t *host);
    void (*message) (struct mc_editor_host_t *host, int flags, const char *title, const char *text);
    void *host_data; /* opaque, owned by editor core */
} mc_editor_host_t;

/* What a plugin provides (callback table) */
typedef struct mc_editor_plugin_t
{
    int api_version;          /* MC_EDITOR_PLUGIN_API_VERSION */
    const char *name;         /* plugin id: "mail", "lsp" */
    const char *display_name; /* UI label */
    mc_ep_flags_t flags;

    /* Required */
    void *(*open) (mc_editor_host_t *host, void *editor_dialog /* opaque */);
    void (*close) (void *plugin_data);

    /* Optional */
    mc_ep_result_t (*activate) (void *plugin_data, void *edit /* opaque */);
    mc_ep_result_t (*configure) (void *plugin_data, void *edit /* opaque */);
    mc_ep_result_t (*handle_action) (void *plugin_data, long command, void *edit /* opaque */);
    mc_ep_result_t (*query_state) (void *plugin_data, void *edit /* opaque */,
                                   mc_ep_state_t *state);
    mc_ep_result_t (*handle_key) (void *plugin_data, int key, void *edit /* opaque */);
    mc_ep_result_t (*handle_event) (void *plugin_data, void *edit /* opaque */, int event_id,
                                    void *payload);
    mc_ep_result_t (*on_file_open) (void *plugin_data, void *edit /* opaque */);
    mc_ep_result_t (*on_file_close) (void *plugin_data, void *edit /* opaque */);
} mc_editor_plugin_t;

typedef const mc_editor_plugin_t *(*mc_editor_plugin_register_fn) (void);

/*** global variables defined in .c file *********************************************************/

/*** declarations of public functions ************************************************************/

/* Registry */
gboolean mc_editor_plugin_add (const mc_editor_plugin_t *plugin);
const GSList *mc_editor_plugin_list (void);
const mc_editor_plugin_t *mc_editor_plugin_find_by_name (const char *name);

/* Loader */
void mc_editor_plugins_load (void);
void mc_editor_plugins_shutdown (void);

/*** inline functions ****************************************************************************/

#endif /* MC__EDITOR_PLUGIN_H */
