/*
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  Base class for wlan engine operations
*
*/


#include "core_operation_base.h"
#include "core_server.h"
#include "core_sub_operation_load_drivers.h"
#include "am_debug.h"

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_operation_base_c::core_operation_base_c(
    core_operation_type_e type,
    u32_t request_id,
    core_server_c* server,
    abs_core_driverif_c* drivers,
    abs_core_server_callback_c* adaptation,
    u32_t feature_flags ) :
    request_id_m( request_id ),
    server_m( server ),
    drivers_m( drivers ),
    adaptation_m( adaptation ),
    is_executing_m( false_t ),
    is_canceling_m( false_t ),
    operation_state_m( core_base_state_init ),
    sub_operation_m( NULL ),
    failure_reason_m( core_error_cancel ),
    operation_type_m( type ),
    feature_flags_m( feature_flags )
    {
    DEBUG1( "core_operation_base_c::core_operation_base_c() (%08X)",
        this );
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_operation_base_c::~core_operation_base_c()
    {
    DEBUG1( "core_operation_base_c::~core_operation_base_c() (%08X)",
        this );
    
    delete sub_operation_m;
    sub_operation_m = NULL;
    server_m = NULL;
    drivers_m = NULL;
    adaptation_m = NULL;
    }
    
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_error_e core_operation_base_c::start_operation()
    {
    DEBUG1( "core_operation_base_c::start_operation() (%08X)",
        this );
    
    is_executing_m = true_t;
    return continue_operation( core_error_ok );
    }
    
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_error_e core_operation_base_c::continue_operation(
    core_error_e request_status )
    {
    DEBUG1( "core_operation_base_c::continue_operation() (%08X)",
        this );
    bool_t is_sub_operation_canceled( false_t );

    /**
     * If we have pending sub-operation, this continue must be for it.
     */
    if ( sub_operation_m )
        {
        core_error_e ret = sub_operation_m->continue_operation( request_status );
        if ( ret == core_error_request_pending )
            {
            DEBUG( "core_operation_base_c::continue_operation() - sub-operation still pending" );
            return ret;
            }

        delete sub_operation_m;
        sub_operation_m = NULL;

        if ( ret == core_error_ok )
            {
            DEBUG( "core_operation_base_c::continue_operation() - sub-operation completed successfully" );
            }
        else if ( is_canceling_m )
            {
            DEBUG( "core_operation_base_c::continue_operation() - sub-operation user cancel completed" );
            is_sub_operation_canceled = true_t;
            }
        else
            {
            DEBUG1( "core_operation_base_c::continue_operation() - sub-operation failed with %u",
                ret );
            failure_reason_m = ret;

            return cancel_operation();
            }
        }

    /**
     * Handle user cancel for operations that haven't overridden the user cancel method.
     */
    if ( operation_state_m == core_base_state_user_cancel )
        {
        DEBUG( "core_operation_base_c::continue_operation() - user cancel complete" );
        ASSERT( is_canceling_m );

        return core_error_cancel;
        }

    /**
     * This continue is for this operation.
     *
     * If a sub-operation has just completed its user cancel, ignore the return
     * value.
     */    
    if ( !is_sub_operation_canceled &&
         request_status != core_error_ok )
        {
        DEBUG1( "core_operation_base_c::continue_operation() - request failed with %u",
            request_status );
        failure_reason_m = request_status;

        return cancel_operation();
        }

    switch ( operation_state_m )
        {
        case core_base_state_init:
            {
            operation_state_m = core_base_state_next;

            if ( is_flags( core_base_flag_drivers_needed ) &&
                 !server_m->get_core_settings().is_driver_loaded() )
                {
                DEBUG( "core_operation_base_c::continue_operation() - drivers required" );
                
                operation_state_m = core_base_state_load_drivers;
                
                core_operation_base_c* operation = new core_sub_operation_load_drivers_c(
                    request_id_m, server_m, drivers_m, adaptation_m );
                return run_sub_operation( operation );
                }

            return next_state();
            }
        case core_base_state_load_drivers:
            {
            DEBUG( "core_operation_base_c::continue_operation() - drivers loaded successfully" );

            operation_state_m = core_base_state_next;

            return next_state();
            }
        default:
            {
            core_error_e ret = next_state(); 

            /**
             * Always return an error code when the operation completes
             * after user cancel.
             */
            if( is_canceling_m &&
                ret != core_error_request_pending )
                {
                DEBUG( "core_operation_base_c::continue_operation() - completing user canceled operation with core_error_cancel" );

                return core_error_cancel;
                }

            return ret;
            }
        }
    }       
     
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_error_e core_operation_base_c::cancel_operation()
    {
    DEBUG1( "core_operation_base_c::cancel_operation() (%08X)",
        this );
    
    /**
     * If we have pending sub-operation, it must be canceled first.
     */
    if ( sub_operation_m )
        {
        core_error_e ret = sub_operation_m->cancel_operation();
        if ( ret == core_error_request_pending )
            {
            DEBUG( "core_operation_base_c::cancel_operation() - sub-operation cancel pending" );
            return ret;
            }

        delete sub_operation_m;
        sub_operation_m = NULL;
        }

    /**
     * If the cancel occurs during the base operation state machine, fail the operation
     * immediately without making the main state machine handle the failure.
     */
    if ( operation_state_m < core_base_state_next )
        {
        DEBUG( "core_operation_base_c::cancel_operation() - canceling using base operation cancel()" );

        return core_operation_base_c::cancel();
        }

    return cancel();
    }
    
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void core_operation_base_c::user_cancel_operation(
    bool_t do_graceful_cancel )
    {
    DEBUG1( "core_operation_base_c::user_cancel_operation() (%08X)",
        this );
    
    /**
     * Prevent double canceling.
     */
    if ( is_canceling_m )
        {
        DEBUG( "core_operation_base_c::user_cancel_operation() - canceling already in progress" );
        
        return;
        }

    /**
     * If we have pending sub-operation, it must be canceled first.
     */
    if ( sub_operation_m )
        {
        sub_operation_m->user_cancel_operation( do_graceful_cancel );
        }

    is_canceling_m = true_t;

    /**
     * If the user cancel occurs during the base operation state machine, fail the operation
     * immediately without making the main state machine handle the user cancel.
     */
    if ( operation_state_m < core_base_state_next )
        {
        DEBUG( "core_operation_base_c::user_cancel_operation() - canceling using base operation user_cancel()" );

        core_operation_base_c::user_cancel( do_graceful_cancel );
        return;
        }
    
    user_cancel( do_graceful_cancel );
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
u32_t core_operation_base_c::request_id() const
    {
    return request_id_m;
    }
    
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_operation_type_e core_operation_base_c::operation_type() const
    {
    return operation_type_m;
    }
    
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
bool_t core_operation_base_c::is_executing() const
    {
    return is_executing_m;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
bool_t core_operation_base_c::is_flags(
    u32_t feature_flags ) const
    {
    if ( feature_flags_m & feature_flags )
        {
        return true_t;
        }

    return false_t;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_error_e core_operation_base_c::cancel()
    {
    DEBUG( "core_operation_base_c::cancel() " );
    
    return failure_reason_m;    
    }        

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void core_operation_base_c::user_cancel(
    bool_t /* do_graceful_cancel */ )
    {
    DEBUG( "core_operation_base_c::user_cancel()" );

    operation_state_m = core_base_state_user_cancel;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_error_e core_operation_base_c::goto_state(
    u32_t state )
    {
    operation_state_m = state;
    
    return next_state();
    }
        
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_error_e core_operation_base_c::run_sub_operation(
    core_operation_base_c* sub_operation )
    {
    DEBUG( "core_operation_base_c::run_sub_operation() " );

    ASSERT( !sub_operation_m );
    sub_operation_m = sub_operation;

    if ( !sub_operation_m )
        {
        DEBUG( "core_operation_base_c::run_sub_operation() - sub-operation creation failed" );
        failure_reason_m = core_error_no_memory;
                   
        return cancel_operation();
        }

    core_error_e ret = sub_operation_m->start_operation();
    if ( ret == core_error_request_pending )
        {
        DEBUG( "core_operation_base_c::run_sub_operation() - sub-operation still pending" );
        return ret;
        }

    delete sub_operation_m;
    sub_operation_m = NULL;

    if ( ret != core_error_ok )
        {
        DEBUG1( "core_operation_base_c::run_sub_operation() - sub-operation failed with %u",
            ret );
        failure_reason_m = ret;
        
        return cancel_operation();               
        }

    DEBUG( "core_operation_base_c::run_sub_operation() - sub-operation completed successfully" );

    return continue_operation( ret );
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_error_e core_operation_base_c::run_sub_operation(
    core_operation_base_c* sub_operation,
    u32_t state )
    {
    operation_state_m = state;

    return run_sub_operation( sub_operation );
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_error_e core_operation_base_c::asynch_goto(
    u32_t state,
    u32_t delay )
    {
    operation_state_m = state;

    server_m->schedule_operation_timer(
        delay );
        
    return core_error_request_pending;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void core_operation_base_c::asynch_default_user_cancel()
    {
    DEBUG( "core_operation_base_c::asynch_default_user_cancel()" );

    server_m->cancel_operation_timer();

    asynch_goto( core_base_state_user_cancel );
    }
