/* -----------------------------------------------------------------------------
 * 
 * Name:    conf.cpp
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
#include "conf.h"
#include <glib.h>
#include <string>

/* Private functions */
int conf_key_file(GKeyFile** keyfile, const char* configfile);

/* Private variables */
const char* ConfigFile = "/home/gabeg/projects/aria/config/aria.conf";

/**
 * Read the configuration file and return the key value as a string 
 */
std::string conf_read(const char* group, const char* key)
{
    GError* err = NULL;
    GKeyFile* keyfile;
    char* value;

    if (conf_key_file(&keyfile, ConfigFile) < 0) {
        return NULL;
    }

    value = g_key_file_get_value(keyfile, group, key, &err);

    if (is_key_err(&err)) {
        return NULL;
    }

    return std::string(value);
}


/**
 * Read the configuration file and return the key value as an unescaped UTF-8
 * string
 */
std::string conf_read_str(const char* group, const char* key)
{
    GError* err = NULL;
    GKeyFile* keyfile;
    char* value;

    if (conf_key_file(&keyfile, ConfigFile) < 0) {
        return NULL;
    }

    value = g_key_file_get_string(keyfile, group, key, &err);

    if (is_key_err(&err)) {
        return NULL;
    }

    return std::string(value);
}

/**
 * Read the configuration file and return the key value as an int
 */
int conf_read_int(const char* group, const char* key)
{
    GError* err = NULL;
    GKeyFile* keyfile;
    gint read;

    if (conf_key_file(&keyfile, ConfigFile) < 0) {
        return -1;
    }

    read = g_key_file_get_integer(keyfile, group, key, &err);

    if (is_key_err(&err)) {
        return -2;
    }

    return read;
}

/**
 * Read the configuration file and return the key value as a bool
 */
bool conf_read_bool(const char* group, const char* key)
{
    GError* err = NULL;
    GKeyFile* keyfile;
    gboolean read;

    if (conf_key_file(&keyfile, ConfigFile) < 0) {
        return -1;
    }

    read = g_key_file_get_boolean(keyfile, group, key, &err);

    if (is_key_err(&err)) {
        return -2;
    }

    return read;
}

/**
 * Return key populated by config file
 */
int conf_key_file(GKeyFile** keyfile, const char* configfile)
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
std::vector<std::string> conf_get_groups(void)
{
    GKeyFile* keyfile;
    char** grouparr;
    size_t length;
    std::vector<std::string> groupvec;

    if (conf_key_file(&keyfile, ConfigFile) < 0) {
        return groupvec;
    }

    grouparr = g_key_file_get_groups(keyfile, &length);
    groupvec.assign(grouparr, grouparr+length);

    return groupvec;
}

/**
 * Return all keys for a given group in the config file
 */
std::vector<std::string> conf_get_keys(const char* group)
{
    GError* err = NULL;
    GKeyFile* keyfile;
    char** keyarr;
    size_t length;
    std::vector<std::string> keyvec;

    if (conf_key_file(&keyfile, ConfigFile) < 0) {
        return keyvec;
    }

    keyarr = g_key_file_get_keys(keyfile, group, &length, &err);

    if (is_key_err(&err)) {
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
        //elmprintf(LOGWARN, (*err)->message);
        g_error_free(*err);
        return 1;
    }

    return 0;
}
