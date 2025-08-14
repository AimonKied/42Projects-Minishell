/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   unset.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: swied <swied@student.42heilbronn.de>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/14 19:03:39 by swied             #+#    #+#             */
/*   Updated: 2025/08/14 20:02:20 by swied            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/execute.h"

int builtin_unset(t_env_list *env_list, t_cmd_node *cmd_node)
{
	t_env_node	*current;

	current = env_list->head;
	if (ft_strcmp(current->variable, cmd_node->cmd[1]) == 0)
	{
		unset_head_node(env_list, current);
		return (0);
	}
	while (current)
	{
		if (ft_strcmp(current->variable, cmd_node->cmd[1]) == 0)
		{
			unset_env_node(env_list, current);
			return (0);
		}	
		current = current->next;
	}
	return (0);
}

void	unset_env_node(t_env_list *env_list, t_env_node *env_node)
{
	t_env_node	*prev;	
	
	prev = env_list->head;
	while (prev->next != env_node && prev)
		prev = prev->next;
	if (prev->next == env_node)
	{
		if (env_node->next)
			prev->next = env_node->next;
		else
			prev->next = NULL;
		gc_free(env_node->value);
		gc_free(env_node->variable);
		gc_free(env_node);
	}
	return ;
}

void	unset_head_node(t_env_list *env_list, t_env_node *env_node)
{
	if (env_node->next)
		env_list->head = env_node->next;
	else
		env_list->head = NULL;
	gc_free(env_node->value);
	gc_free(env_node->variable);
	gc_free(env_node);
}
