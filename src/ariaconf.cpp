/* -----------------------------------------------------------------------------
 * 
 * Name:    ariaconf.cpp
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

/* Includes */
#include "ariadef.hpp"
#include "ariaconf.hpp"
#include <glib.h>
#include <iostream>
#include <string>
#include <cstdlib>
#include <cstring>

/*
 *
 * HAVE CHECK FOR NULL WHEN RETURNING FROM THESE FUNCTIONS
 * HAVE std::string BE INPUT PARAMETERS FOR FUNCTIONS
 *
 */


ARIA_NAMESPACE

namespace config
{
    /**
     * Read the configuration file and return the key value as a string 
     */
    std::string read(const char* group, const char* key)
    {
        GError* err = NULL;
        GKeyFile* keyfile;
        char* value;

        if (config::new_key_file(&keyfile, ARIA_CONFIG_FILE) < 0) {
            return "";
        }

        value = g_key_file_get_value(keyfile, group, key, &err);

        if (config::is_key_err(&err)) {
            return "";
        }

        return std::string(value);
    }

    /**
     * Read the configuration file and return the key value as a string 
     */
    std::string read(const char* key)
    {
        return config::read("Main", key);
    }

    /**
     * Read the configuration file and return the key value as an unescaped UTF-8
     * string
     */
    std::string read_str(const char* group, const char* key)
    {
        GError* err = NULL;
        GKeyFile* keyfile;
        char* value;

        if (config::new_key_file(&keyfile, ARIA_CONFIG_FILE) < 0) {
            return "";
        }

        value = g_key_file_get_string(keyfile, group, key, &err);

        if (config::is_key_err(&err)) {
            return "";
        }

        return std::string(value);
    }

    /**
     * Read the configuration file and return the key value as an int
     */
    int read_int(const char* group, const char* key)
    {
        GError* err = NULL;
        GKeyFile* keyfile;
        gint read;

        if (config::new_key_file(&keyfile, ARIA_CONFIG_FILE) < 0) {
            return -1;
        }

        read = g_key_file_get_integer(keyfile, group, key, &err);

        if (config::is_key_err(&err)) {
            return -2;
        }

        return read;
    }

    /**
     * Read the configuration file and return the key value as a bool
     */
    bool read_bool(const char* group, const char* key)
    {
        GError* err = NULL;
        GKeyFile* keyfile;
        gboolean read;

        if (config::new_key_file(&keyfile, ARIA_CONFIG_FILE) < 0) {
            return -1;
        }

        read = g_key_file_get_boolean(keyfile, group, key, &err);

        if (config::is_key_err(&err)) {
            return -2;
        }

        return read;
    }

    /**
     * Return key populated by config file
     */
    int new_key_file(GKeyFile** keyfile, const char* configfile)
    {
        GKeyFileFlags flags = G_KEY_FILE_NONE;
        GError* err = NULL;

        *keyfile = g_key_file_new();

        if (!g_key_file_load_from_file(*keyfile, configfile, flags, &err)) {
            is_key_err(&err);
            *keyfile = NULL;
            return -1;
        }

        return 0;
    }

    /**
     * Return all groups in the config file
     */
    std::vector<std::string> get_groups(void)
    {
        GKeyFile* keyfile;
        char** grouparr;
        size_t length;
        std::vector<std::string> groupvec;

        if (config::new_key_file(&keyfile, ARIA_CONFIG_FILE) < 0) {
            return groupvec;
        }

        grouparr = g_key_file_get_groups(keyfile, &length);
        groupvec.assign(grouparr, grouparr+length);

        return groupvec;
    }

    /**
     * Return all keys for a given group in the config file
     */
    std::vector<std::string> get_keys(const char* group)
    {
        GError* err = NULL;
        GKeyFile* keyfile;
        char** keyarr;
        size_t length;
        std::vector<std::string> keyvec;

        if (config::new_key_file(&keyfile, ARIA_CONFIG_FILE) < 0) {
            return keyvec;
        }

        keyarr = g_key_file_get_keys(keyfile, group, &length, &err);

        if (config::is_key_err(&err)) {
            return keyvec;
        }

        keyvec.assign(keyarr, keyarr+length);

        return keyvec;
    }

    /**
     * Check if an error occurred
     */
    int is_key_err(GError **err)
    {
        if (*err) {
            g_error_free(*err);
            return 1;
        }
        return 0;
    }
}

ARIA_NAMESPACE_END
