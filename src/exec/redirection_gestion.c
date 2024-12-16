/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redirection_gestion.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: apoet <apoet@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/04 17:34:17 by febouana          #+#    #+#             */
/*   Updated: 2024/12/15 04:09:30 by apoet            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

//*OKOK
//? Initialise un fichier pour une redirection donnée
//? parmi <, > et >>
int	init_file(t_cmd *cmd, char *filename, int type)
{
	if (!cmd || !filename)
		return (ERROR);
	if ((type == INPUT || type == HEREDOC) && cmd->infile > -2)
		close(cmd->infile);
	if ((type == TRUNC || type == APPEND) && cmd->outfile > -2)
		close(cmd->outfile);
	if (type == INPUT)
		cmd->infile = open(filename, O_RDONLY);
	if (type == TRUNC)
		cmd->outfile = open(filename, O_CREAT | O_WRONLY | O_TRUNC, 0644);
	if (type == APPEND)
		cmd->outfile = open(filename, O_CREAT | O_WRONLY | O_APPEND, 0644);
	if (cmd->infile == -1 || cmd->outfile == -1)
	{
		perror("Error: File operation failed");
		return (ERROR);
	}
	return (SUCCESS);
}

// int	fill_cmd_nodes_redirections(t_cmd *cmd, t_token *token,  t_token **real_token)
// {
// 	bool is_pipe;
// 	int is_access;
	
// 	is_pipe = false;
// 	if (token->type == HEREDOC && token->next->type == ARG)
// 		if (heredoc(cmd, token->next->value) != SUCCESS)
// 			return (ERROR);
// 	if (token->type == INPUT || token->type == TRUNC || token->type >= APPEND)
// 	{
// 		is_access = access(token->next->value, F_OK);
// 		if (is_access == 0)
// 		{
// 			if (init_file(cmd, token->next->value, token->type) != SUCCESS)
// 				return (ERROR);
// 		}
// 		else
// 			return(print_error("SIUminishell: "), print_error(token->next->value), cleanup(1, ": No such file or directory\n", NO_EXIT, 2));
// 	}
// 	if (token->prev && token->prev->type == PIPE)
// 		is_pipe = true;
// 	if (token->next->next) // MON PRECIEUX
// 		token = token->next->next;
// 	if ((!cmd->prev || is_pipe == true) && token->type == ARG && (check_command_in_path(token->value) == SUCCESS || is_builtin(token->value) == true))
// 		(*real_token)->next->next->type = CMD;  
// 	else if (!token->next && !cmd->cmd_param && token->type == ARG) 
// 		return(print_error("SIUminishell: "), print_error(token->value), cleanup(1, ": command not found\n", NO_EXIT, 2)); 
// 	return (SUCCESS);
// }

int	fill_cmd_nodes_redirections(t_cmd *cmd, t_token **real_token)
{
	bool is_pipe;
	int is_access;
	
	is_pipe = false;
	if ((*real_token)->type == HEREDOC && (*real_token)->next->type == ARG)
		if (heredoc(cmd, (*real_token)->next->value) != SUCCESS)
			return (ERROR);
	if ((*real_token)->type == INPUT || (*real_token)->type == TRUNC || (*real_token)->type >= APPEND)
	{
		is_access = access((*real_token)->next->value, F_OK);
		if (is_access == 0)
		{
			if (init_file(cmd, (*real_token)->next->value, (*real_token)->type) != SUCCESS)
				return (ERROR);
		}
		else
			return(print_error("SIUminishell: "), print_error((*real_token)->next->value), cleanup(1, ": No such file or directory\n", NO_EXIT, 2));
	}
	if ((*real_token)->prev && (*real_token)->prev->type == PIPE)
		is_pipe = true;
	printf("1VERIF-IN==%s | %d\n\n", (*real_token)->value, (*real_token)->type);
	if ((*real_token)->next->next) // MON PRECIEUX
		(*real_token) = (*real_token)->next->next;
	printf("2VERIF-IN==%s | %d\n\n", (*real_token)->value, (*real_token)->type);
	if ((!cmd->prev || is_pipe == true) && (*real_token)->type == ARG && (check_command_in_path((*real_token)->value) == SUCCESS || is_builtin((*real_token)->value) == true))
		(*real_token)->type = CMD;  
	// else if (!(*real_token)->next && !cmd->cmd_param && (*real_token)->type == ARG) 
	// 	return(print_error("SIUminishell: "), print_error((*real_token)->value), cleanup(1, ": command not found\n", NO_EXIT, 2)); 
	return (SUCCESS);
}

int	close_all_redi(t_data *data)
{
	t_cmd	*cmd;

	cmd = data->cmd;
	while (cmd)
	{
		if (data->pip[1] >= 0)
			close(data->pip[1]);
		if (data->pip[0] >= 0)
			close(data->pip[0]);
		if (cmd->outfile >= 0)
			close(cmd->outfile);
		if (cmd->infile >= 0)
			close(cmd->infile);
		cmd = cmd->next;
	}
	return (SUCCESS);
}

void	close_all_redi_of_each_nodes(t_data *data)
{
	t_cmd	*cmd;

	if (!data)
		return ;
	if (data->pip[0] >= 0)
		close(data->pip[0]);
	if (data->pip[1] >= 0)
		close(data->pip[1]);
	cmd = data->cmd;
	while (cmd)
	{
		if (cmd->cmd_param)
			ft_free_multi_array(cmd->cmd_param);
		if (cmd->outfile >= 0)
			close(cmd->outfile);
		if (cmd->infile >= 0)
			close(cmd->infile);
		data->current_pid = 0;
		cmd = cmd->next;
	}
	cmd = NULL;
	// printf("WATI-FREE-SUCCESS\n\n");
	// free les nodes (+ prev next) une par une en + ou deja fait ?
}
