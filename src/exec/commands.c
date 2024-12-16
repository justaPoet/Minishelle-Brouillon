/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   commands.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: apoet <apoet@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/02 18:43:47 by obouayed          #+#    #+#             */
/*   Updated: 2024/12/16 20:19:20 by apoet            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

//*OKOK
//? Compte les paramètres de commande
int count_cmd_param(t_token *og_token)
{
    t_token *token = og_token;
    unsigned int i = 0;

    while (token && token->type != PIPE)
    {
        if (token->type >= 1 && token->type <= 4)
        {    
            if (token->next && token->next->type == ARG)
                i--;
        }        
        else
            i++;
        token = token->next;
    }
    return (i);
}

//*OKOK
//? Initialise les paramètres de commande
char **init_cmd_param(t_token *og_token)
{
    char **cmd_param;
    int i;
    t_token *token;

    token = og_token;
    i = count_cmd_param(token);
    if (i == 0 || i == FAILURE)
        return (NULL);
    cmd_param = malloc(sizeof(char *) * (i + 1));
    if (!cmd_param)
        return (NULL); //! cleanup ERR_MALLOC
    cmd_param[i] = NULL;
    i = 0;
    while (token && token->type != PIPE)
    {
        if (token->type >= 1 && token->type <= 4)
        {
            if (token->next && token->next->type == ARG)
                token = token->next->next;
			else
                return (NULL);
        }
        else
        {            
            cmd_param[i] = ft_strdup(token->value);
            if (!cmd_param[i])
                return (NULL); //! cleanup ERR_MALLOC
            i++;
            token = token->next;
        }
    }
    return (cmd_param);
}

void close_null_sq()
{
    t_data * data;
    t_cmd *tmp;
    
    data = get_data();
    tmp = data->cmd;
    while (tmp)
    {
        if (!tmp->cmd_param)
        {  
            if (tmp->infile > -2)
            {
                close(tmp->infile);
                tmp->infile = -2;
            }
            if (tmp->outfile > -2)
            {
                close(tmp->outfile);
                tmp->outfile = -2;
            }
            tmp->skip_cmd = true;
        }
        tmp = tmp->next;
    }
}

int verif_before_init_cmd_param(t_token *token, t_cmd *cmd)
{
    if (token->type != CMD && token->type != ARG)
        return (ERROR);
    if (token->type == CMD && cmd->cmd_param)
        return (ERROR);
    if (token->type == ARG)
    {
        if (token->prev && (token->prev->type >= 1 && token->prev->type <= 4))
            return (ERROR);
        if (cmd->cmd_param)
            return (ERROR);   
    }
    if (check_command_in_path(token->value) == ERROR && is_builtin(token->value) == false && access(token->value, X_OK) != 0)
        return(print_error("minishell: "), print_error(token->value), print_error(": command not found\n"), FAILURE); //! quitte et stop le while de token 
    return (SUCCESS);
}

//! FAIRE UNE FN SPECIALE AVEC TOUS LES CAS SPECIAUX 
//* "< infile < infile" ==> OKOK
//* "< infile cat < infile -e" ==> OKOK
//* "< infile iwqej < infile" ==> "command not found"
//* "< infile < infile iwqej" ==> "command not found"
//* "< infile cat < infile ls" ==> "ls: No such file or directory"
//* "< infile wedjkfbn < hjbfjnwef" ==> "hjbfjnwef: No such file or directory"
//* "<< o << p"
//* "<< o << p cat -e"

//*OKOK
//? Remplit chaque noeud de cmd avec les bonnes redirections et
//? initialise cmd_param, si besoin.
int fill_cmd_nodes(t_data *data)
{
    int new_cmd;
    t_cmd *cmd;
    t_token *token;

    token = data->token;
    cmd = data->cmd;
    while (token && cmd)
    {
        if ((token->type >= 1 && token->type <= 4) && token->next && token->next->type == ARG)
            if (fill_cmd_nodes_redirections(cmd, &token) == ERROR) //if (fill_cmd_nodes_redirections(cmd, &token) == ERROR)
                return(ERROR); 
        if (token->type == PIPE)
            cmd = cmd->next;
        new_cmd = verif_before_init_cmd_param(token, cmd);
        if (new_cmd == SUCCESS)
        {    
            cmd->cmd_param = init_cmd_param(token);
            if (!cmd->cmd_param)
                return (ERROR); //! cleanup ERR_MALLOC
        }
        if (new_cmd == FAILURE)
            return (close_null_sq(), cleanup(1, NULL, NO_EXIT, 2), ERROR);
    	token = token->next;
    }
    return (close_null_sq(), SUCCESS);
}

//*OKOK
//? Initialise les nœuds de commandes 
int init_cmd_nodes(t_data *data)
{
    t_cmd *cmd;
    t_token *token;
    int nb_sq;

    token = data->token;
    nb_sq = count_nb_sequences(token);
    if (nb_sq == FAILURE)
        return (ERROR);
    while (nb_sq > 0)
    {
        cmd = malloc(sizeof(t_cmd));
        if (!cmd)
            return (cleanup(ERROR, ERR_MALLOC, ERROR, 2));
        cmd->infile = -2;
        cmd->outfile = -2;
        cmd->cmd_param = NULL;
        cmd->next = NULL;
        cmd->prev = NULL;
        cmd->skip_cmd = false;
        if (!data->cmd)
            data->cmd = cmd;
        else
            add_cmd_to_list(cmd, data->cmd);
        nb_sq--;
    }
    return (SUCCESS);
}

int check_access_redirections(t_data *data)
{
    t_token *token;

    token = data->token;
    while (token)
    {
        if (token->type == INPUT || token->type == TRUNC || token->type == APPEND)
        {
            if (token->next && token->next->type == ARG)
            {
                if (access(token->next->value, F_OK) != 0)
                    return(print_error("minishell: "), print_error(token->next->value), cleanup(1, ": No such file or directory\n", NO_EXIT, 2), ERROR);
            }
            else    
                return(ERROR); 
        }
    	token = token->next;
    }
    return (SUCCESS);
}

int init_cmd(t_data *data)
{
    if (check_access_redirections(data) == ERROR)
        return (ERROR);
    if (init_cmd_nodes(data) == ERROR)
        return (ERROR); 
    if (fill_cmd_nodes(data) == ERROR)
        return (ERROR); 
    printf("yoooooo bro\n\n");
// //? test sip_cmd
    // t_cmd * tmp = data->cmd;
    //     printf("1=%s\n", tmp->cmd_param[0]);
    //     printf("1=%s\n", tmp->cmd_param[1]);
    //     printf("1=%s\n", tmp->cmd_param[2]);
    //     tmp = tmp->next;
    //     printf("2=%s\n", tmp->cmd_param[0]);
    //     printf("2=%s\n", tmp->cmd_param[1]);
    //     printf("2=%s\n", tmp->cmd_param[2]);
    //     tmp = tmp->next;
    //     printf("3=%s\n", tmp->cmd_param[0]);
    //     printf("3=%s\n", tmp->cmd_param[1]);
    //     printf("3=%s\n", tmp->cmd_param[2]);

    // free_all_cmd_nodes(&data->cmd);        

    return (SUCCESS);
}























//! difference entre compter les cmd et les sequences... 
//! ...donc plus trop utile
// int cmd_counter_ce_gros_bg_la()
// {
//     int i;
//     t_data* data;
//     t_token *tmp;

//     data = get_data();
//     tmp = data->token;
//     i = 0;
//     while (tmp)
//     {
//         if ((tmp->type >= 1 && tmp->type <= 4) && (!tmp->prev || tmp->prev->type == PIPE) && tmp->next && tmp->next->next && tmp->next->next->type == ARG)
//             if (check_command_in_path(tmp->next->next->value) == SUCCESS || is_builtin(tmp->next->next->value) == true)
//                 i++;
//         if (tmp->type == CMD)
//             i++;
//         tmp = tmp->next;
//     } 
//     return (i);
// }

//! difference entre compter les cmd et les sequences... doinc plus trop utile
// int cmd_counter_ce_gros_bg_la()
// {
//     int i;
//     t_data* data;
//     t_token *tmp;

//     data = get_data();
//     tmp = data->token;
//     i = 0;
//     while (tmp)
//     {
//         if ((tmp->type >= 1 && tmp->type <= 4) && (!tmp->prev || tmp->prev->type == PIPE) && tmp->next && tmp->next->next && tmp->next->next->type == ARG)
//             if (check_command_in_path(tmp->next->next->value) == SUCCESS || is_builtin(tmp->next->next->value) == true)
//                 i++;
//         if (tmp->type == CMD)
//             i++;
//         tmp = tmp->next;
//     } 
//     return (i);
// }

