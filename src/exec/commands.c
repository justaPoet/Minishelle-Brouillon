/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   commands.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: apoet <apoet@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/02 18:43:47 by obouayed          #+#    #+#             */
/*   Updated: 2024/12/16 15:51:54 by apoet            ###   ########.fr       */
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
        return (print_error(ERR_MALLOC), NULL); //! cleanup ERR_MALLOC
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
                return (print_error(ERR_MALLOC), NULL); //! cleanup ERR_MALLOC
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



bool verif_before_init_cmd_param()
{

    if (token->type == CMD)
    {
        
    }


    
    printf("OKAY OKAY == %s \n", token->value);
            if (check_command_in_path(token->value) == ERROR && is_builtin(token->value) == false)
                return(print_error("AUDDminishell: "), print_error(token->value), cleanup(1, ": command not found\n", NO_EXIT, 2), ERROR); //! 
}

//*OKOK
//? Remplit chaque noeud de cmd avec les bonnes redirections et
//? initialise cmd_param, si besoin.
int fill_cmd_nodes(t_data *data)
{
    t_cmd *cmd;
    t_token *token;

    token = data->token;
    cmd = data->cmd;
    while (token && cmd)
    {
        if ((token->type >= 1 && token->type <= 4) && token->next && token->next->type == ARG)
        {   
            if (fill_cmd_nodes_redirections(cmd, &token) == ERROR)
                return(ERROR);
        }  
        if (token->type == PIPE)
            cmd = cmd->next;
        if (token->type == CMD || token->type == ARG)
        {   
            // if (token->type == CMD)
            //     if ()
			
            if (verif_before_init_cmd_param() == true)
            {    
                cmd->cmd_param = init_cmd_param(token);
                if (!cmd->cmd_param)
                    return (ERROR); //besoin de free les precedents noeud si deja init
            }
        }
    	token = token->next;
    }
    close_null_sq();
    return (SUCCESS);
}

//! FAIRE UNE FN SPECIALE AVEC TOUS LES CAS SPECIAUX 
//! "< infile < infile"
//! "< infile cat < infile -e"
//! "< infile iwqej < infile"
//! "< infile < infile iwqej"

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
        cmd->infile= -2;
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

int init_cmd(t_data *data)
{
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

//! "< infile < infile wejinfkejnf" ==> "command not found"






















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

