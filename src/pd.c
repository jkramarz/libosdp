/**
 *  SPDX-License-Identifier: MIT
 *
 *  Author: Siddharth Chandrasekaran
 *    Date: Tue Sep 24 23:09:20 IST 2019
 */

#include <stdlib.h>
#include <string.h>
#include "common.h"

osdp_pd_t *osdp_pd_setup(int num_pd, osdp_pd_info_t *p)
{
    int fc;
    pd_t *pd;
    cp_t *cp;
    osdp_t *ctx;
    struct pd_cap * cap;

    ctx = calloc(1, sizeof(osdp_t));
    if (ctx == NULL) {
        osdp_log(LOG_ERR, "Failed to alloc osdp_t");
        goto malloc_err;
    }
    ctx->magic = 0xDEADBEAF;

    ctx->cp = calloc(1, sizeof(cp_t));
    if (ctx->cp == NULL) {
        osdp_log(LOG_ERR, "Failed to alloc cp_t");
        goto malloc_err;
    }
    cp = to_cp(ctx);
    child_set_parent(cp, ctx);
    cp->num_pd = 1;

    ctx->pd = calloc(1, sizeof(pd_t));
    if (ctx->pd == NULL) {
        osdp_log(LOG_ERR, "Failed to alloc cp_t[]");
        goto malloc_err;
    }

    set_current_pd(ctx, 0);
    pd = to_pd(ctx, 0);
    child_set_parent(pd, ctx);
    pd->baud_rate = p->baud_rate;
    pd->address = p->address;
    pd->flags = p->init_flags;
    pd->seq_number = -1;
    pd->send_func = p->send_func;
    pd->recv_func = p->recv_func;
    memcpy(&pd->id, &p->id, sizeof(struct pd_id));
    cap = pd->cap;
    while (cap && cap->function_code > 0) {
        fc = pd->cap->function_code;
        if (fc >= CAP_SENTINEL)
            break;
        pd->cap[fc].function_code = cap->function_code;
        pd->cap[fc].compliance_level = cap->compliance_level;
        pd->cap[fc].num_items = cap->num_items;
        cap++;
    }

    set_flag(pd, PD_FLAG_PD_MODE);

    osdp_set_log_level(LOG_WARNING);
    osdp_log(LOG_INFO, "pd setup complete");
    return (osdp_pd_t *)ctx;

malloc_err:
    osdp_pd_teardown((osdp_pd_t *)ctx);
    return NULL;
}

void osdp_pd_teardown(osdp_pd_t *ctx)
{
    cp_t *cp;
    pd_t *pd;

    if (ctx == NULL)
        return;

    /* teardown pd */
    pd = to_pd(ctx, 0);
    if (pd != NULL)
        free(pd);

    /* teardown cp */
    cp = to_cp(ctx);
    if (cp != NULL) {
        free(cp);
    }

    free(ctx);
}
