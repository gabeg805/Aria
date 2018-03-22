/* -----------------------------------------------------------------------------
 * 
 * Name:    ariaconf.h
 * Author:  Gabriel Gonzalez
 * Email:   gabeg@bu.edu
 * License: The MIT License (MIT)
 * 
 * Description: Parse config files.
 * 
 * Notes: None.
 * 
 * -----------------------------------------------------------------------------
 */

/* Header guard */
#ifndef ARIA_CONF_H
#define ARIA_CONF_H

/* Includes */
#include "ariadef.h"
#include <glib.h>
#include <string>
#include <vector>

ARIA_NAMESPACE

namespace config
{
    std::string read(const char* group, const char* key);
    std::string read(const char* key);
    std::string read_str(const char* group, const char* key);
    int read_int(const char* group, const char* key);
    bool read_bool(const char* group, const char* key);
    int new_key_file(GKeyFile** keyfile, const char* configfile);
    std::vector<std::string> get_groups(void);
    std::vector<std::string> get_keys(const char* group);
    int is_key_err(GError **err);
}

ARIA_NAMESPACE_END

#endif /* ARIA_CONF_H */
