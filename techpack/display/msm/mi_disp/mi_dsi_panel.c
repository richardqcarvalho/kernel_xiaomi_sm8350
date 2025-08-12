/* SPDX-License-Identifier: GPL-2.0-only */
/*
 * Copyright (c) 2020, The Linux Foundation. All rights reserved.
 * Copyright (C) 2020 XiaoMi, Inc.
 */

#define pr_fmt(fmt)	"mi-dsi-panel:[%s:%d] " fmt, __func__, __LINE__
#include <linux/delay.h>
#include <linux/slab.h>
#include <linux/gpio.h>
#include <linux/of_gpio.h>
#include <linux/rtc.h>
#include <linux/pm_wakeup.h>
#include <video/mipi_display.h>

#include "dsi_panel.h"
#include "dsi_display.h"
#include "dsi_ctrl_hw.h"
#include "dsi_parser.h"
#include "mi_dsi_display.h"

int mi_dsi_panel_write_cmd_set(struct dsi_panel *panel,
				struct dsi_panel_cmd_set *cmd_sets)
{
	int rc = 0, i = 0;
	ssize_t len;
	struct dsi_cmd_desc *cmds;
	u32 count;
	enum dsi_cmd_set_state state;
	struct dsi_display_mode *mode;
	const struct mipi_dsi_host_ops *ops = panel->host->ops;

	if (!panel || !panel->cur_mode) {
		return -EINVAL;
	}

	mode = panel->cur_mode;

	cmds = cmd_sets->cmds;
	count = cmd_sets->count;
	state = cmd_sets->state;

	if (count == 0) {
		goto error;
	}

	for (i = 0; i < count; i++) {
		if (state == DSI_CMD_SET_STATE_LP)
			cmds->msg.flags |= MIPI_DSI_MSG_USE_LPM;

		if (cmds->last_command)
			cmds->msg.flags |= MIPI_DSI_MSG_LASTCOMMAND;

		len = ops->transfer(panel->host, &cmds->msg);
		if (len < 0) {
			rc = len;
			goto error;
		}
		if (cmds->post_wait_ms)
			usleep_range(cmds->post_wait_ms * 1000,
					((cmds->post_wait_ms * 1000) + 10));
		cmds++;
	}
error:
	return rc;
}
