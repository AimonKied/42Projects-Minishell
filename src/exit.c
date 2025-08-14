/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exit.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: swied <swied@student.42heilbronn.de>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/03 10:58:06 by swied             #+#    #+#             */
/*   Updated: 2025/08/14 23:41:57 by swied            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/execute.h"

/* not done yet */
int	builtin_exit(t_cmd_node *cmd_node)
{
	int	i;

	if (!cmd_node || !cmd_node->cmd)
		return (1);
	i = 0;
	while (cmd_node->cmd[i])
		i++;
	if (i == 1)
	{
		free_all_garbage();
		ft_putstr_fd("exit\n", 1);
		exit(0);
	}
	else if (i == 2)
		exit_with_arg(cmd_node);
	else
		return (ft_putstr_fd("minishell: exit: too many arguments", 2), 1);
	return (0);
}

void	exit_with_arg(t_cmd_node *cmd_node)
{
	int	exit_nb;

	exit_nb = 0;
	if (valid_args(cmd_node->cmd[1]) == 1)
	{
		ft_putstr_fd("exit\n", 1);
		ft_putstr_fd("minishell: exit: numeric argument required\n", 2);
		free_all_garbage();
		exit(2);
	}
	exit_nb = get_exit_code(cmd_node->cmd[1]);
	free_all_garbage();
	exit(exit_nb);
}

int	valid_args(const char *str)
{
	int	i;

	i = 0;
	while (str[i])
	{
		if (!((str[i] >= '0' && str[i] <= '9')
				|| (i == 0 && (str[i] == '-' || str[i] == '+'))))
			return (1);
		i++;
	}
	return (0);
}

int	get_exit_code(char *str)
{
	int	sign;
	int	exit_code;

	sign = 1;
	if (*str == '+' || *str == '-')
	{
		if (*str == '-')
			sign *= -1;
		str++;
	}
	exit_code = 0;
	while (*str >= '0' && *str <= '9')
	{
		exit_code = (exit_code * 10) + ((*str - '0') * sign);
		str++;
	}
	return (exit_code);
}
