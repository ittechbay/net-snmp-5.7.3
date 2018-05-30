/*
 * Note: this file originally auto-generated by mib2c using
 *  $
 */

#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-includes.h>
#include <net-snmp/agent/net-snmp-agent-includes.h>
#include "ftsPtpTable.h"

/** Initializes the ftsPtpTable module */
void
init_ftsPtpTable(void)
{
  /* here we initialize all the tables we're planning on supporting */
    initialize_table_ftsPtpTable();
}


/** Initialize the ftsPtpTable table by defining its contents and how it's structured */
void
initialize_table_ftsPtpTable(void)
{
    const oid ftsPtpTable_oid[] = {1,3,6,1,4,1,63000,2,1,13,1,2,1,11,2};
    const size_t ftsPtpTable_oid_len   = OID_LENGTH(ftsPtpTable_oid);
    netsnmp_handler_registration    *reg;
    netsnmp_tdata                   *table_data;
    netsnmp_table_registration_info *table_info;

    DEBUGMSGTL(("ftsPtpTable:init", "initializing table ftsPtpTable\n"));

    reg = netsnmp_create_handler_registration(
              "ftsPtpTable",     ftsPtpTable_handler,
              ftsPtpTable_oid, ftsPtpTable_oid_len,
              HANDLER_CAN_RWRITE
              );

    table_data = netsnmp_tdata_create_table( "ftsPtpTable", 0 );
    if (NULL == table_data) {
        snmp_log(LOG_ERR,"error creating tdata table for ftsPtpTable\n");
        return;
    }
    table_info = SNMP_MALLOC_TYPEDEF( netsnmp_table_registration_info );
    if (NULL == table_info) {
        snmp_log(LOG_ERR,"error creating table info for ftsPtpTable\n");
        return;
    }
    netsnmp_table_helper_add_indexes(table_info,
                           ASN_INTEGER,  /* index: ftsPtpIndex */
                           0);

    table_info->min_column = COLUMN_FTSPTPINDEX;
    table_info->max_column = COLUMN_FTSPTPDELAYREQS;
    
    netsnmp_tdata_register( reg, table_data, table_info );

    /* Initialise the contents of the table here */
}

    /* Typical data structure for a row entry */
struct ftsPtpTable_entry {
    /* Index values */
    long ftsPtpIndex;

    /* Column values */
    long ftsPtpIndex;
    long ftsPtpState;
    long ftsPtpDomain;
    long old_ftsPtpDomain;
    long ftsPtpDelayReqs;
    long ftsPtpDelayReqs;

    int   valid;
};

/* create a new row in the table */
netsnmp_tdata_row *
ftsPtpTable_createEntry(netsnmp_tdata *table_data
                 , long  ftsPtpIndex
                ) {
    struct ftsPtpTable_entry *entry;
    netsnmp_tdata_row *row;

    entry = SNMP_MALLOC_TYPEDEF(struct ftsPtpTable_entry);
    if (!entry)
        return NULL;

    row = netsnmp_tdata_create_row();
    if (!row) {
        SNMP_FREE(entry);
        return NULL;
    }
    row->data = entry;

    DEBUGMSGT(("ftsPtpTable:entry:create", "row 0x%x\n", (uintptr_t)row));
    entry->ftsPtpIndex = ftsPtpIndex;
    netsnmp_tdata_row_add_index( row, ASN_INTEGER,
                                 &(entry->ftsPtpIndex),
                                 sizeof(entry->ftsPtpIndex));
    if (table_data)
        netsnmp_tdata_add_row( table_data, row );
    return row;
}

/* remove a row from the table */
void
ftsPtpTable_removeEntry(netsnmp_tdata     *table_data, 
                 netsnmp_tdata_row *row) {
    struct ftsPtpTable_entry *entry;

    if (!row)
        return;    /* Nothing to remove */

    DEBUGMSGT(("ftsPtpTable:entry:remove", "row 0x%x\n", (uintptr_t)row));

    entry = (struct ftsPtpTable_entry *)row->data;
    SNMP_FREE( entry );   /* XXX - release any other internal resources */

    if (table_data)
        netsnmp_tdata_remove_and_delete_row( table_data, row );
    else
        netsnmp_tdata_delete_row( row );    
}


/** handles requests for the ftsPtpTable table */
int
ftsPtpTable_handler(
    netsnmp_mib_handler               *handler,
    netsnmp_handler_registration      *reginfo,
    netsnmp_agent_request_info        *reqinfo,
    netsnmp_request_info              *requests) {

    netsnmp_request_info       *request;
    netsnmp_table_request_info *table_info;
    netsnmp_tdata              *table_data;
    netsnmp_tdata_row          *table_row;
    struct ftsPtpTable_entry          *table_entry;
    int                         ret;

    DEBUGMSGTL(("ftsPtpTable:handler", "Processing request (%d)\n", reqinfo->mode));

    switch (reqinfo->mode) {
        /*
         * Read-support (also covers GetNext requests)
         */
    case MODE_GET:
        for (request=requests; request; request=request->next) {
            if (request->processed)
                continue;

            table_entry = (struct ftsPtpTable_entry *)
                              netsnmp_tdata_extract_entry(request);
            table_info  =     netsnmp_extract_table_info( request);
    
            switch (table_info->colnum) {
            case COLUMN_FTSPTPINDEX:
                if ( !table_entry ) {
                    netsnmp_set_request_error(reqinfo, request,
                                              SNMP_NOSUCHINSTANCE);
                    continue;
                }
                snmp_set_var_typed_integer( request->requestvb, ASN_INTEGER,
                                            table_entry->ftsPtpIndex);
                break;
            case COLUMN_FTSPTPSTATE:
                if ( !table_entry ) {
                    netsnmp_set_request_error(reqinfo, request,
                                              SNMP_NOSUCHINSTANCE);
                    continue;
                }
                snmp_set_var_typed_integer( request->requestvb, ASN_INTEGER,
                                            table_entry->ftsPtpState);
                break;
            case COLUMN_FTSPTPDOMAIN:
                if ( !table_entry ) {
                    netsnmp_set_request_error(reqinfo, request,
                                              SNMP_NOSUCHINSTANCE);
                    continue;
                }
                snmp_set_var_typed_integer( request->requestvb, ASN_INTEGER,
                                            table_entry->ftsPtpDomain);
                break;
            case COLUMN_FTSPTPDELAYREQS:
                if ( !table_entry ) {
                    netsnmp_set_request_error(reqinfo, request,
                                              SNMP_NOSUCHINSTANCE);
                    continue;
                }
                snmp_set_var_typed_integer( request->requestvb, ASN_INTEGER,
                                            table_entry->ftsPtpDelayReqs);
                break;
            case COLUMN_FTSPTPDELAYREQS:
                if ( !table_entry ) {
                    netsnmp_set_request_error(reqinfo, request,
                                              SNMP_NOSUCHINSTANCE);
                    continue;
                }
                snmp_set_var_typed_integer( request->requestvb, ASN_INTEGER,
                                            table_entry->ftsPtpDelayReqs);
                break;
            default:
                netsnmp_set_request_error(reqinfo, request,
                                          SNMP_NOSUCHOBJECT);
                break;
            }
        }
        break;

        /*
         * Write-support
         */
    case MODE_SET_RESERVE1:
        for (request=requests; request; request=request->next) {
            if (request->processed)
                continue;

            table_entry = (struct ftsPtpTable_entry *)
                              netsnmp_tdata_extract_entry(request);
            table_info  =     netsnmp_extract_table_info( request);
    
            switch (table_info->colnum) {
            case COLUMN_FTSPTPDOMAIN:
                /* or possibly 'netsnmp_check_vb_int_range' */
                ret = netsnmp_check_vb_int( request->requestvb );
                if ( ret != SNMP_ERR_NOERROR ) {
                    netsnmp_set_request_error( reqinfo, request, ret );
                    return SNMP_ERR_NOERROR;
                }
                break;
            default:
                netsnmp_set_request_error( reqinfo, request,
                                           SNMP_ERR_NOTWRITABLE );
                return SNMP_ERR_NOERROR;
            }
        }
        break;

    case MODE_SET_RESERVE2:
        break;

    case MODE_SET_FREE:
        break;

    case MODE_SET_ACTION:
        for (request=requests; request; request=request->next) {
            if (request->processed)
                continue;

            table_entry = (struct ftsPtpTable_entry *)
                              netsnmp_tdata_extract_entry(request);
            table_info  =     netsnmp_extract_table_info( request);
    
            switch (table_info->colnum) {
            case COLUMN_FTSPTPDOMAIN:
                table_entry->old_ftsPtpDomain = table_entry->ftsPtpDomain;
                table_entry->ftsPtpDomain     = *request->requestvb->val.integer;
                break;
            }
        }
        break;

    case MODE_SET_UNDO:
        for (request=requests; request; request=request->next) {
            if (request->processed)
                continue;

            table_entry = (struct ftsPtpTable_entry *)
                              netsnmp_tdata_extract_entry(request);
            table_row   =     netsnmp_tdata_extract_row(  request);
            table_data  =     netsnmp_tdata_extract_table(request);
            table_info  =     netsnmp_extract_table_info( request);
    
            switch (table_info->colnum) {
            case COLUMN_FTSPTPDOMAIN:
                table_entry->ftsPtpDomain     = table_entry->old_ftsPtpDomain;
                table_entry->old_ftsPtpDomain = 0;
                break;
            }
        }
        break;

    case MODE_SET_COMMIT:
        break;
    }
    return SNMP_ERR_NOERROR;
}
