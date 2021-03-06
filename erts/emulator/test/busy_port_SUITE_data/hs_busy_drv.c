/*
 * %CopyrightBegin%
 * 
 * Copyright Ericsson AB 2009. All Rights Reserved.
 * 
 * The contents of this file are subject to the Erlang Public License,
 * Version 1.1, (the "License"); you may not use this file except in
 * compliance with the License. You should have received a copy of the
 * Erlang Public License along with this software. If not, it can be
 * retrieved online at http://www.erlang.org/.
 * 
 * Software distributed under the License is distributed on an "AS IS"
 * basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See
 * the License for the specific language governing rights and limitations
 * under the License.
 * 
 * %CopyrightEnd%
 */

#include <errno.h>
#include "erl_driver.h"

ErlDrvData start(ErlDrvPort port, char *command);
void output(ErlDrvData drv_data, char *buf, int len);
int control(ErlDrvData drv_data, unsigned int command, char *buf, 
	    int len, char **rbuf, int rlen); 

static ErlDrvEntry busy_drv_entry = { 
    NULL /* init */,
    start,
    NULL /* stop */,
    output,
    NULL /* ready_input */,
    NULL /* ready_output */,
    ERTS_TEST_BUSY_DRV_NAME,
    NULL /* finish */,
    NULL /* handle */,
    control,
    NULL /* timeout */,
    NULL /* outputv */,
    NULL /* ready_async */,
    NULL /* flush */,
    NULL /* call */,
    NULL /* event */,
    ERL_DRV_EXTENDED_MARKER,
    ERL_DRV_EXTENDED_MAJOR_VERSION,
    ERL_DRV_EXTENDED_MINOR_VERSION,
    ERTS_TEST_BUSY_DRV_FLAGS,
    NULL /* handle2 */,
    NULL /* handle_monitor */,
    NULL /* stop_select */
};

DRIVER_INIT(busy_drv)
{
    return &busy_drv_entry;
}

ErlDrvData start(ErlDrvPort port, char *command)
{
    return (ErlDrvData) port;
}

void output(ErlDrvData drv_data, char *buf, int len)
{
    int res;
    ErlDrvPort port = (ErlDrvPort) drv_data;
    ErlDrvTermData msg[] = {
	ERL_DRV_PORT,	driver_mk_port(port),
	ERL_DRV_ATOM,	driver_mk_atom("caller"),
	ERL_DRV_PID,	driver_caller(port),
	ERL_DRV_TUPLE,	(ErlDrvTermData) 3
    };
    res = driver_output_term(port, msg, sizeof(msg)/sizeof(ErlDrvTermData));
    if (res <= 0)
	driver_failure_atom(port, "driver_output_term failed");
}

int control(ErlDrvData drv_data, unsigned int command, char *buf, 
	    int len, char **rbuf, int rlen)
{
    switch (command) {
    case 'B': /* busy */
	set_busy_port((ErlDrvPort) drv_data, 1);
	break;
    case 'N': /* not busy */
	set_busy_port((ErlDrvPort) drv_data, 0);
	break;
    default:
	driver_failure_posix((ErlDrvPort) drv_data, EINVAL);
	break;
    }
    return 0;
}
