#include <yed/plugin.h>

#define LINE_LEN 1024

void init_command_history( void );

void save_command_history( yed_event *event );

unsigned int command_history_limit;

int yed_plugin_boot(yed_plugin *self)
{
    YED_PLUG_VERSION_CHECK();

    yed_event_handler yeh;
    yeh.kind = EVENT_PRE_QUIT;
    yeh.fn = save_command_history;

    yed_plugin_add_event_handler( self, yeh );

    if ( yed_get_var( "command-history-limit" ) == NULL )
    {
        yed_set_var( "command-history-limit", "1000" );
    }
    else
    {
        yed_get_var_as_int( "command-history-limit", &command_history_limit );

        if ( command_history_limit > 10000 )
        {
            command_history_limit = 10000;
            yed_set_var( "command-history-limit", "10000" );
        }
    }

    init_command_history();

    return 0;
}

void init_command_history( void )
{
    FILE *f;
    char *path;
    char line[LINE_LEN];
    char *cmd;
    int n_cmds = 0;

    LOG_FN_ENTER();

    path = get_config_item_path( "my-command-history.txt" );
    f = fopen( path, "r" );
    free( path );

    if ( !f )
    {
        yed_log( "failed to load command-history" );
        LOG_EXIT();
        return;
    }

    while ( fgets( line, LINE_LEN, f ) != NULL )
    {
        if ( line[strlen( line ) - 1] == '\n' )
        {
            line[strlen( line ) - 1] = 0;
        }

        cmd = strdup( line );

        array_insert( ys->cmd_prompt_hist, 0, cmd );
        n_cmds++;
    }

    fclose( f );

    yed_log( "command-history initialized: %d commands loaded", n_cmds );
    LOG_EXIT();
}

void save_command_history( yed_event *event )
{
    FILE *f;
    char *path;
    char **cit;
    int n_cmds = 0;

    LOG_FN_ENTER();

    path = get_config_item_path( "my-command-history.txt" );
    f = fopen( path, "w" );
    free( path );

    if ( !f )
    {
        yed_log( "failed to save command-history" );
        LOG_EXIT();
        return;
    }

    array_rtraverse( ys->cmd_prompt_hist, cit )
    {
        fprintf( f, "%s\n", *cit );
        n_cmds++;

        if ( n_cmds == command_history_limit )
        {
            break;
        }
    }

    fclose( f );

    yed_log( "command-history saved: %d commands saved", array_len( ys->cmd_prompt_hist ) );
    LOG_EXIT();
}
