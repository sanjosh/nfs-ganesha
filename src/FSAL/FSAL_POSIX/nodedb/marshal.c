/*
 * Copyright (C) Paul Sheer, 2012
 * Author: Paul Sheer paulsheer@gmail.com
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *
 * ------------- 
 */



#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdarg.h>

#define MAX_TOKENS              1024



static int lineno = 0;
static const char *filename = "nodedb.c";
static char *marshal_ops[256];
static int n_ops = 0;

static void err (const char *fmt, ...)
{
    char x[256];
    va_list ap;
    va_start (ap, fmt);
    vsprintf (x, fmt, ap);
    va_end (ap);
    fprintf (stderr, "%s:%d: %s\n", filename, lineno, x);
    exit (1);
}

static const char *tokens[MAX_TOKENS];

struct token {
    int number;
    int len;
    const char *text;
    const char *p;
};

struct token token[MAX_TOKENS + 1];

enum {
    __FIRST_TOKEN = 256,
    VOID,
    CONST,
    STRUCT,
    LONG_LONG,
    LONG,
    INT,
    CHAR,
    FILE_DATA,
    HANDLE_DATA,
    STAT,
    NODEDB,

    __LAST_TOKEN = MAX_TOKENS - 1,
    IDENTIFIER
};

static void init_tokens (void)
{
    int i, j;

    memset (tokens, '\0', sizeof (tokens));
    memset (token, '\0', sizeof (token));

    tokens['('] = "(";
    tokens[')'] = ")";
    tokens[','] = ",";
    tokens['*'] = "*";
    tokens[VOID] = "void";
    tokens[CONST] = "const";
    tokens[STRUCT] = "struct";
    tokens[LONG_LONG] = "long long";
    tokens[LONG] = "long";
    tokens[INT] = "int";
    tokens[CHAR] = "char";
    tokens[FILE_DATA] = "file_data";
    tokens[HANDLE_DATA] = "handle_data";
    tokens[STAT] = "stat";
    tokens[NODEDB] = "nodedb";

    j = 0;

    for (i = 0; i < MAX_TOKENS; i++) {
        if (tokens[i]) {
            token[j].number = i;
            token[j].text = tokens[i];
            token[j].len = strlen (tokens[i]);
            j++;
        }
    }
}

#define IS_ALPHA(c)     (((c) >= 'a' && (c) <= 'z') || ((c) >= 'A' && (c) <= 'Z') || ((c) == '_'))
#define IS_NUMBER(c)    ((c) >= '0' && (c) <= '9')
#define IS_IDCHAR(c)    (IS_NUMBER(c) || IS_ALPHA(c))

static int match_identifier (const char *p, int *l)
{
    *l = 0;
    if (!IS_ALPHA (*p))
        return 0;
    while (IS_IDCHAR (*p)) {
        p++;
        (*l)++;
    }
    return 1;
}

static struct token *next_token (const char *p)
{
    int i;
    static struct token m;
    for (i = 0; token[i].text; i++) {
        m = token[i];
        if (!strncmp (p, m.text, m.len) && (!IS_IDCHAR (p[0]) || !IS_IDCHAR (p[m.len])))
            return &m;
    }
    if (match_identifier (p, &m.len)) {
        char *s;
        s = malloc (m.len + 1);
        strncpy (s, p, m.len);
        s[m.len] = '\0';
        m.text = s;
        m.number = IDENTIFIER;
        return &m;
    }
    return NULL;
}

static void tokenize (const char *p, struct token *out)
{
    while (*p) {
        struct token *m;
        if ((m = next_token (p))) {
            *out = *m;
            out->p = p;
            p += m->len;
            out++;
        } else {
            if (*p <= ' ')
                p++;
            else {
                err ("unrecognized token: %.40s", p);
                exit (1);
            }
        }
    }
    memset (out, '\0', sizeof (*out));
}

#if 0
(*encode_func_t) (char *param_name);
(*decode_func_t) (char *param_name);
#endif

enum param_type {
    IN,
    OUT,
    INOUT,
    IGNORE,
    ENCAPSULATE
};

struct phrase {
    enum param_type param_type;
    int tokenid[10];
    int identifier;
    int deref;
    const char *encode;
    const char *decode;
    const char *cleanup;
    struct token *tokens;
    int alloced_on_the_stack;
};

static void output_decl (void)
{
    printf ("/* This file is autogenerated -- do not edit */\n");
    printf ("/* This file is autogenerated -- do not edit */\n");
    printf ("/* This file is autogenerated -- do not edit */\n");
    printf ("/* This file is autogenerated -- do not edit */\n");
    printf ("\n\n");
    printf ("#include <pthread.h>\n");
    printf ("\n\n");
    printf ("#include <stdlib.h>\n");
    printf ("#include <stdio.h>\n");
    printf ("#include <string.h>\n");
    printf ("#include <sys/types.h>\n");
    printf ("#include <sys/stat.h>\n");
    printf ("#include <unistd.h>\n");
    printf ("\n\n");

    printf ("struct file_data;\n");
    printf ("struct stat;\n");
    printf ("struct nodedb;\n\n");
    printf ("struct connection;\n\n");

    printf ("#include \"sockbuf.h\"\n");
    printf ("#include \"nodedb.h\"\n");
    printf ("#include \"connection.h\"\n");
    printf ("#include \"connectionpool.h\"\n");
    printf ("#include \"ops.h\"\n");
    printf ("#include \"encoding.h\"\n");
    printf ("\n\n");

    printf ("\n\n");
    printf ("\n\n");

}

struct phrase phrase[100] = {
    {INOUT, {STRUCT, FILE_DATA, '*', IDENTIFIER, 0}, 3, 1, "encode_file_data(conn, %s)", "decode_file_data(conn, &%s)", NULL},
    {IN, {CONST, STRUCT, FILE_DATA, '*', IDENTIFIER, 0}, 4, 1, "encode_file_data(conn, %s)", "decode_file_data(conn, &%s)", NULL},
    {INOUT, {STRUCT, HANDLE_DATA, '*', IDENTIFIER, 0}, 3, 1, "encode_handle_data(conn, %s)", "decode_handle_data(conn, &%s)", NULL},
    {IN, {CONST, STRUCT, HANDLE_DATA, '*', IDENTIFIER, 0}, 4, 1, "encode_handle_data(conn, %s)", "decode_handle_data(conn, &%s)", NULL},
    {INOUT, {STRUCT, STAT, '*', IDENTIFIER, 0}, 3, 1, "encode_stat(conn, %s)", "decode_stat(conn, &%s)", NULL},
    {IN, {CONST, STRUCT, STAT, '*', IDENTIFIER, 0}, 4, 1, "encode_stat(conn, %s)", "decode_stat(conn, &%s)", NULL},
    {IN, {INT, IDENTIFIER, 0}, 1, 0, "encode_int(conn, &%s)", "decode_int(conn, &%s)", NULL},
    {ENCAPSULATE, {STRUCT, NODEDB, '*', IDENTIFIER, 0}, 3, 0, NULL, NULL, NULL},
    {IGNORE, {VOID, IDENTIFIER, 0}, 1, 0, NULL, NULL, NULL},
    {IGNORE, {VOID, 0}, 0, 0, NULL, NULL, NULL},
    {OUT, {CHAR, '*', '*', IDENTIFIER, 0}, 3, 1, "encode_char_p(conn, *%s)", "decode_char_p(conn, %s)", "free_char_p(%s)"},
    {IN, {CONST, CHAR, '*', IDENTIFIER, 0}, 3, 0, "encode_char_p(conn, %s)", "decode_char_p(conn, &%s)", "free_char_p(&%s)"},
    {IGNORE, {0}},
};

static int phrase_length (struct phrase *p)
{
    int i;
    for (i = 0; p->tokenid[i]; i++);
    return i;
}

static int match_phrase (struct token *t, struct phrase *p)
{
    int i;
    for (i = 0; t[i].number == p->tokenid[i] && t[i].number && p->tokenid[i]; i++);
    return !p->tokenid[i];
}

static struct phrase *find_phrase (struct token *t)
{
    int i;

    for (i = 0; phrase[i].tokenid[0]; i++) {
        if (match_phrase (t, &phrase[i])) {
            struct phrase *r;
            r = (struct phrase *) malloc (sizeof (*r));
            *r = phrase[i];
            r->tokens = t;
            return r;
        }
    }

    return NULL;
}

static void out (const char *fmt, ...)
{
    static int space = 0;
    char x[256];
    va_list ap;
    va_start (ap, fmt);
    vsprintf (x, fmt, ap);
    va_end (ap);
    if (x[0] <= ' ' || x[0] == ',' || x[0] == ')')
        space = 0;
    else if (x[0] == '(')
        space = 1;
    printf ("%s%s", space ? " " : "", x);
    if (x[0]) {
        int c;
        c = x[strlen (x) - 1];
        if (c <= ' ')
            space = 0;
        else if (IS_IDCHAR (c))
            space = 1;
        else if (c == ',')
            space = 1;
        else
            space = 0;
    }
}

static void get_method_params (struct token *line_tokens, struct phrase *method, struct phrase **params)
{
    struct token *c;
    int n = 0;

    c = line_tokens + method->identifier + 1;

    if (c->number != '(')
        err ("expecting '(': %.20s...", c->p);

    c++;

    while (c->number != ')') {
        int is_pointer = 0;
        struct phrase *param;

        param = find_phrase (c);

        if (!param)
            err ("unknown token phrase: %.20s...", c->p);

        params[n++] = param;

        c += phrase_length (param);
        if (c->number == ',')
            c++;

        if (param->param_type == IN || param->param_type == OUT || param->param_type == INOUT) {
            int j;
            for (j = 0; j < param->identifier - param->deref; j++)
                is_pointer += (param->tokenid[j] == '*');
            param->alloced_on_the_stack = !is_pointer || !param->deref;
        }
    }

    params[n] = NULL;
}

static void output_return_type (struct phrase *method, int deref)
{
    struct token *c;
    int i;

    for (c = method->tokens, i = 0; i < method->identifier - (deref ? method->deref : 0); i++, c++)
        out ("%s", c->text);
}

static void decl_arg_list (struct phrase **params)
{
    int i;

    out ("struct connection_pool *connpool", "");

    for (i = 0; params[i]; i++) {
        int j;
        struct phrase *param;
        const char *param_name;

        param = params[i];
        param_name = param->tokens[param->identifier].text;

        if (param->param_type == IN || param->param_type == OUT || param->param_type == INOUT) {
            out (",");
            for (j = 0; j < param->identifier; j++)
                if (param->tokenid[j] != CONST)
                    out ("%s", param->tokens[j].text);
            out ("var__%s", param_name);
        }
    }
}

static void output_vars (int decode, struct phrase **params)
{
    int i;

    for (i = 0; params[i]; i++) {
        struct phrase *param;

        param = params[i];

        if (!param->encode)
            continue;

        if (param->param_type == OUT || param->param_type == INOUT) {
            const char *param_name;
            char par_str[128];

            param_name = param->tokens[param->identifier].text;

            out ("    conn->progress = \"%s\";\n", param_name);
            out ("    ");
            if (decode) {
                sprintf (par_str, "var__%s", param_name);
                out (param->decode, par_str);
            } else {
                sprintf (par_str, "%svar__%s", param->alloced_on_the_stack ? "" : "&", param_name);
                out (param->encode, par_str);
            }
            out (";\n");
        }
    }

    out ("\n");
}

static void input_vars (int decode, struct phrase **params)
{
    int i;

    for (i = 0; params[i]; i++) {
        struct phrase *param;

        param = params[i];

        if (!param->decode)
            continue;

        if (param->param_type == IN || param->param_type == INOUT) {
            const char *param_name;
            char par_str[128];

            param_name = param->tokens[param->identifier].text;

            out ("    conn->progress = \"%s\";\n", param_name);
            out ("    ");
            if (decode) {
                sprintf (par_str, "%svar__%s", param->alloced_on_the_stack ? "" : "&", param_name);
                out (param->decode, par_str);
            } else {
                sprintf (par_str, "var__%s", param_name);
                out (param->encode, par_str);
            }
            out (";\n");
        }
    }
}

static void declare_vars (struct phrase **params)
{
    int i;

    for (i = 0; params[i]; i++) {
        int j, is_pointer = 0;
        struct phrase *param;

        param = params[i];

        if (param->param_type == IN || param->param_type == OUT || param->param_type == INOUT) {
            const char *param_name;
            const char *stars;

            param_name = param->tokens[param->identifier].text;

            out ("    ");
            for (j = 0; j < param->identifier - param->deref; j++) {
                if (param->tokenid[j] != CONST) {
                    out ("%s", param->tokens[j].text);
                    is_pointer += (param->tokenid[j] == '*');
                }
            }
            stars = "**********" + (10 - is_pointer - 1);
            if (param->deref) {
                if (is_pointer)
                    out ("var__%s = NULL;\n", param_name);
                else if (param->deref)
                    out ("vard__%s, %svar__%s = &vard__%s;\n", param_name, stars, param_name, param_name);
            } else {
                out ("var__%s;\n", param_name);
            }
        }
    }

    out ("\n");
}



static void call_method (struct phrase **params, int atomic, struct phrase *method,
                         const char *method_name, int method_returns_pointer, int method_returns_void)
{
    int i;

    out ("\n    decode_endvars(conn);\n");

    out ("\n    if (decode_error(conn))\n        return 1;\n");

    if (atomic)
        out ("\n    nodedb_lock(conn->db);\n");
    if (method->deref && method_returns_pointer) {
        out ("    r_ = %s(", method_name);
    } else {
        out ("    %s%s%s(", method_returns_void ? "" : "r = ", method_returns_pointer ? "*" : "", method_name);
    }

    for (i = 0; params[i]; i++) {
        struct phrase *param;
        const char *param_name;

        param = params[i];

        param_name = param->tokens[param->identifier].text;

        if (param->param_type == IGNORE)
            out ("%s", param_name);
        else if (param->param_type == ENCAPSULATE)
            out ("conn->%s", param_name);
        else
            out ("%svar__%s", (!param->alloced_on_the_stack && param->deref) ? "&" : "", param_name);
        if (params[i + 1])
            out (",");
    }

    out (");\n");

    if (method->deref && method_returns_pointer)
        out ("    if(r_)\n        r = *r_;\n");

    if (atomic)
        out ("    nodedb_unlock(conn->db);\n\n");

    if (method->deref && method_returns_pointer)
        out ("    if(!r_) {\n        encode_null(conn);\n        return encode_error(conn);\n    }\n\n");

    if (!method_returns_void) {
        out ("    conn->progress = \"return\";\n", "");
        out ("    ");
        out (method->encode, method->deref ? "&r" : "r");
        out (";\n");
    }

    out ("\n");
}

static void free_vars (struct phrase **params, struct phrase *method)
{
    int i;

    for (i = 0; params[i]; i++) {
        struct phrase *param;
        const char *param_name;
        char par_str[128];

        param = params[i];

        if (!param->cleanup)
            continue;

        param_name = param->tokens[param->identifier].text;

        out ("    ");
        sprintf (par_str, "%svar__%s", param->alloced_on_the_stack ? "" : "&", param_name);
        out (param->cleanup, par_str);
        out (";\n");
    }

    if (method->cleanup) {
        out ("    ");
        out (method->cleanup, "r");
        out (";\n");
    }
}


static void private_prototype (struct token *line_tokens)
{
    int i;

    for (i = 0; line_tokens[i].text; i++)
        out ("%s", line_tokens[i].text);
    out (";\n\n");
}


static void marshaller_prototype (struct phrase **params, struct phrase *method, const char *method_name,
                                  int method_returns_pointer, int method_returns_void)
{
    out ("\n");
    output_return_type (method, 0);
    out ("MARSHAL_%s(", method_name);
    decl_arg_list (params);
    out ("); /* @PROTOTYPE@ */ \n\n");
}


static void marshaller (struct phrase **params, struct phrase *method, const char *method_name,
                        int method_returns_pointer, int method_returns_void)
{
    output_return_type (method, 0);

    out ("MARSHAL_%s(", method_name);

    decl_arg_list (params);

    out (")\n{\n");

    if (!method_returns_void) {
        out ("    ");
        output_return_type (method, 0);
        out ("r;\n\n");
    }

    out ("    struct locked_connection *lockedconn;\n\n");
    out ("    struct connection *conn;\n\n");
    out ("    lockedconn = connection_pool_get_locked(connpool);\n\n");
    out ("    conn = lockedconn->conn;\n\n");

    out ("    encode_op(conn, MARSHAL_op_%s);\n\n", method_name);

    input_vars (0, params);

    out ("\n\n");
    out ("\n    encode_endvars(conn);\n");
    out ("\n    encode_flush(conn);\n");
    out ("\n\n");

    if (method->deref && method_returns_pointer)
        out ("    if(decode_null(conn)) {\n        locked_connection_unlock(lockedconn);\n        return NULL;\n    }\n\n");

    if (!method_returns_void) {
        char par_str[128];

        if (method->deref && method_returns_pointer)
            out ("    r = malloc(sizeof(*r));\n");

        out ("    conn->progress = \"return\";\n", "");
        out ("    ");
        sprintf (par_str, "r");
        out (method->decode, par_str);
        out (";\n");
    }

    output_vars (1, params);

    out ("    locked_connection_unlock(lockedconn);\n\n");

    if (!method_returns_void)
        out ("    return r;\n");

    out ("}\n\n");
}



static void demarshaller (struct phrase **params, struct phrase *method, const char *method_name,
                          int method_returns_pointer, int method_returns_void, int atomic)
{
    out ("int DEMARSHAL_%s (struct connection *conn)\n{\n", method_name);

    if (!method_returns_void) {
        out ("    ");
        output_return_type (method, 1);
        out ("r%s;\n", (method->deref && method_returns_pointer) ? ", *r_" : "");
    }

    out ("\n");

    declare_vars (params);

    input_vars (1, params);

    call_method (params, atomic, method, method_name, method_returns_pointer, method_returns_void);

    output_vars (0, params);

    free_vars (params, method);

    out ("    return encode_error(conn);\n}\n\n");
}



static void process_line (char *p, int atomic)
{
    struct token line_tokens[1024];
    struct phrase *method;
    const char *method_name;
    struct phrase *params[64];
    int method_returns_pointer = 0;
    int method_returns_void;

    tokenize (p, line_tokens);

    private_prototype (line_tokens);

    method = find_phrase (line_tokens);
    if (!method) {
        err ("unknown token phrase: %.20s...", line_tokens->p);
        exit (1);
    }

    method_returns_pointer = (method->tokenid[method->identifier - 1] == '*');
    method_name = method->tokens[method->identifier].text;

    marshal_ops[n_ops] = (char *) malloc (strlen (method_name) + 1);
    strcpy (marshal_ops[n_ops], method_name);
    n_ops++;

    method_returns_void = (method->param_type == IGNORE);
    get_method_params (line_tokens, method, params);
    marshaller_prototype (params, method, method_name, method_returns_pointer, method_returns_void);
    marshaller (params, method, method_name, method_returns_pointer, method_returns_void);
    demarshaller (params, method, method_name, method_returns_pointer, method_returns_void, atomic);
}

static void dump_ops (void)
{
    int i;
    FILE *f;
    f = fopen ("ops.h", "w");
    if (!f) {
        perror ("ops.h");
        exit (1);
    }

    fprintf (f, "/* This file is autogenerated -- do not edit */\n");
    fprintf (f, "/* This file is autogenerated -- do not edit */\n");
    fprintf (f, "/* This file is autogenerated -- do not edit */\n");
    fprintf (f, "/* This file is autogenerated -- do not edit */\n");
    fprintf (f, "\n\n");

    fprintf (f, "\n\nenum ops {\n");
    for (i = 0; i < n_ops; i++)
        fprintf (f, "    MARSHAL_op_%s%s\n", marshal_ops[i], i == n_ops - 1 ? "" : ",");
    fprintf (f, "};\n\n");
    fclose (f);
}

static void ops_output (void)
{
    int i;

    dump_ops ();

    out ("int demarshal(struct connection *conn)\n{\n");

    out ("    enum ops op;\n\n");
    out ("    decode_op(conn, &op);\n");
    out ("    if (decode_error(conn))\n        return conn->input.save_errno == -1 ? -1 : 1;\n\n");

    out ("    switch (op) {\n");
    for (i = 0; i < n_ops; i++) {
        out ("    case MARSHAL_op_%s:\n", marshal_ops[i]);
        out ("        return DEMARSHAL_%s(conn);\n", marshal_ops[i]);
    }
    out ("    }\n\n");
    out ("    encode_unknown(conn);\n");
    out ("    return encode_error(conn);\n");
    out ("}\n\n");
}

int main (int argc, char **argv)
{
    FILE *f;
    char line[1024];


    f = fopen (filename, "r");
    if (!f) {
        perror (filename);
        return 1;
    }

    init_tokens ();

    output_decl ();

    while (fgets (line, sizeof (line), f)) {
        char *p;
        lineno++;
        p = line;
        while (*p && *p <= ' ')
            p++;
        if (!*p)
            continue;
        if (!strncmp (p, "MARSHALATOMIC", 13)) {
            if (line[strlen (line) - 2] != ')')
                err ("MARSHALATOMIC method declarations be on a line ending in )");
            process_line (p + 13, 1);
        }
    }

    ops_output ();

    return 0;
}

