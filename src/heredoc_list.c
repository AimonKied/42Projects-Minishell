/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc_list.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: swied <swied@student.42heilbronn.de>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/29 01:39:48 by swied             #+#    #+#             */
/*   Updated: 2025/08/08 18:30:04 by swied            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/execute.h"

/* Creates a hd_list and fills it with nodes while going through cmd_list */
int	create_hd_list(t_cmd_list *cmd_list)
{
	t_cmd_node	*cmd_node;
	t_file_node	*file_node;

	cmd_node = cmd_list->head;
	while (cmd_node)
	{
		cmd_node->hd_list = gc_malloc (sizeof(t_hd_list));
		if (!cmd_node->hd_list)
			return (-1);
		cmd_node->hd_list->head = NULL;
		cmd_node->hd_list->tail = NULL;
		cmd_node->hd_list->size = 0;
		file_node = cmd_node->file->head;
		while (file_node)
		{
			if (file_node->redir_type == REDIR_HEREDOC)
			{
				if (create_hd(file_node->filename, cmd_node, file_node) == -1)
					return (-1);
			}
			file_node = file_node->next;
		}
		cmd_node = cmd_node->next;
	}
	return (0);
}

/* Creates a hd_node with helper function and connects it to the hd_list */
int	create_hd(char *lim, t_cmd_node *cmd_node, t_file_node *file_node)
{
	t_hd_node	*hd_node;

	hd_node = create_hd_node(lim);
	if (!hd_node)
		return (-1);
	hd_node->file_node = file_node;
	if (!cmd_node->hd_list->head)
	{
		cmd_node->hd_list->head = hd_node;
		cmd_node->hd_list->tail = hd_node;
		cmd_node->hd_list->size = 1;
	}
	else
	{
		cmd_node->hd_list->tail->next = hd_node;
		cmd_node->hd_list->tail = hd_node;
		cmd_node->hd_list->size++;
	}
	if (create_child_for_hd(lim, hd_node) == -1)
		return (-1);
	return (0);
}

/* allocates hd_node + givese lim */
t_hd_node	*create_hd_node(char *lim)
{
	t_hd_node	*new_hd_node;

	new_hd_node = gc_malloc(sizeof(t_hd_node));
	if (!new_hd_node)
		return (NULL);
	new_hd_node->lines = NULL;
	new_hd_node->lim = ft_strdup(lim);
	if (!new_hd_node->lim)
		return (NULL);
	add_to_gc(new_hd_node->lim);
	new_hd_node->file_node = NULL;
	new_hd_node->next = NULL;
	return (new_hd_node);
}

/* creates a child process where input gets collected | in parent a function stores the input from child into hd_node */
int create_child_for_hd(char *del, t_hd_node *hd_node)
{
	int		pipe_fd[2];
	int		status;
	pid_t	child_pid;

	if (pipe(pipe_fd) == -1)
		return (-1);
	child_pid = fork();
	if (child_pid == -1)
		return (close(pipe_fd[0]), close(pipe_fd[1]), -1);
	if (child_pid == 0)
	{
		close(pipe_fd[0]);
		get_input_from_user(del, pipe_fd[1]);
	}
	else
	{
		close(pipe_fd[1]);
		if (store_from_input(hd_node, pipe_fd[0]) == -1)	
			return (close(pipe_fd[0]), waitpid(child_pid, &status, 0), -1);
		close(pipe_fd[0]);
		// return (check_exit_status(child_pid));
		return (0);
	}
	return (0);
}

/* gets input on stdin and writes the line into write_fd */
int	get_input_from_user(char *del, int write_fd)
{
	char	*line;
	char	*expanded_line;

	while (1)
	{
		write(1, "> ", 2);
		line = get_next_line(0);
		if (!line)
			break ;
		if (is_delimiter(line, del))	
			break ;
		expanded_line = ft_strdup(line);
		// instead of ft_strdup(line) the expansion function
		free(line);
		if (!expanded_line)
			return(close(write_fd), exit(1), 1);
		write(write_fd, expanded_line, ft_strlen(expanded_line));
		write(write_fd, "\n", 1);
		free(expanded_line);
	}
	return (close(write_fd), exit(0), 0);
}

/* gets the line from read_fd and adds it into the hd_node */
int	store_from_input(t_hd_node *hd_node, int read_fd)
{
	char	*line;
	char	*line_copy;
	int		line_len;

	while(1)
	{
		line = get_next_line(read_fd);
		if (!line)
			break ;
		line_len = ft_strlen(line);
		if (line_len > 0 && line[line_len - 1] == '\n')
			line[line_len - 1] = '\0';
		line_copy = ft_strdup(line);
		if (!line_copy)
			return (free(line), -1);
		add_to_gc(line_copy);
		add_line_to_hd_node(hd_node, line_copy);
		free(line);
	}
	return (0);
}

/* adds a line to a hd_node */
void	add_line_to_hd_node(t_hd_node *hd_node, char *line)
{
	t_hd_line	*new_line;
	t_hd_line	*current;

	new_line = create_hd_line(line);
	if (!new_line)
		return ;
	if (!hd_node->lines)
	{
		hd_node->lines = new_line;
		return ;
	}	
	current = hd_node->lines;
	while (current->next)
		current = current->next;
	current->next = new_line;
}

/* creates and allocates a hd_line and fills the argument */
t_hd_line	*create_hd_line(char *content)
{
	t_hd_line	*new_line;

	new_line = gc_malloc(sizeof(t_hd_line));
	if (!new_line)
		return (NULL);
	new_line->line = content;
	new_line->next = NULL;
	return (new_line);
}
