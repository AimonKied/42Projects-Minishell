/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc_execute.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: swied <swied@student.42heilbronn.de>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/11 13:02:47 by swied             #+#    #+#             */
/*   Updated: 2025/08/11 13:45:09 by swied            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/execute.h"

/* creates a child proccess and calls a function in the child to fill the pipe from hd_list | returns fd of it */
int	get_heredoc(t_cmd_node *cmd_node)
{
	int		pipe_fd[2];
	pid_t	child_pid;

	if (pipe(pipe_fd) == -1)
		return (-1);
	child_pid = fork();
	if (child_pid == -1)
		return (close(pipe_fd[0]), close(pipe_fd[1]), -1);
	if (child_pid == 0)
	{
		close(pipe_fd[0]);
		fill_pipe_from_list(cmd_node, pipe_fd[1]);
		close(pipe_fd[1]);
		exit(0);
	}
	else
	{
		close(pipe_fd[1]);
		waitpid(child_pid, NULL, 0);
		return (pipe_fd[0]);
	}
	return (pipe_fd[0]);
}

/* fills the write_fd with the lines of the last hd_node in hd_list */
void	fill_pipe_from_list(t_cmd_node *cmd_node, int write_fd)
{
	t_hd_node	*current_hd;
	t_hd_line	*hd_line;

	current_hd = cmd_node->hd_list->head;
	while (current_hd && current_hd->next)
		current_hd = current_hd->next;
	hd_line = current_hd->lines;
	while (hd_line)
	{
		write(write_fd, hd_line->line, ft_strlen(hd_line->line));
		write(write_fd, "\n", 1);
		hd_line = hd_line->next;
	}
}
