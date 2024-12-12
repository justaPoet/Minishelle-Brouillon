/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_exit.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: obouayed <obouayed@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/01 23:27:50 by obouayed          #+#    #+#             */
/*   Updated: 2024/12/11 18:48:51 by obouayed         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"
// static int is_overflowing(const char *str)
// {
// 	long result = 0;
// 	int sign = 1;
// 	int i = 0;

// 	if (!str)
// 		return 0;
// 	if (str[i] == '-' || str[i] == '+')
// 	{
// 		if (str[i] == '-')
// 			sign = -1;
// 		i++;
// 	}
// 	while (str[i])
// 	{
// 		if (!ft_isdigit(str[i]))
// 			return 0;
// 		if (sign > 0)
// 		{
// 			if (result > LONG_MAX / 10 || 
// 				(result == LONG_MAX / 10 && (str[i] - '0') > LONG_MAX % 10))
// 				return 0;
// 		}
// 		else
// 		{
// 			if (result > -(LONG_MIN / 10) ||
// 				(result == -(LONG_MIN / 10) && (str[i] - '0') > -(LONG_MIN % 10)))
// 				return 0;
// 		}

// 		result = result * 10 + (str[i] - '0');
// 		i++;
// 	}

// 	// Check final range for exit code (0-255)
// 	result *= sign;
// 	if (result < 0 || result > 255)
// 		return 0;

// 	return 1;
// }

long	ft_atol_exit(char *str)
{
	unsigned int	i;
	int				sign;
	long long		result;

	i = 0;
	sign = 1;
	result = 0;
	while (str[i] == ' ' || (str[i] >= 9 && str[i] <= 13))
		i++;
	if (str[i] == '-' || str[i] == '+')
	{
		if (str[i] == '-')
			sign = -1;
		i++;
	}
	while (str[i] >= '0' && str[i] <= '9')
	{
		result = result * 10 + (str[i] - '0');
		i++;
	}
	return (result * sign);
}

//! exit status KOKO
int	ft_exit(char **cmd_param)
{
	int		i;
	t_data	*data;
	long	exit_value;

	i = 0;
	data = get_data();
	if (!cmd_param[1])
		return (cleanup(NO_CHANGE, "exit\n", data->exit_status, 1));
	// if (!is_overflowing(cmd_param[1]))
	// 	return (print_error("minishell: exit: "), print_error(cmd_param[1]),
	// 		cleanup(2, " numeric argument required\n", 2, 2));
	exit_value = ft_atol_exit(cmd_param[1]);
	if (cmd_param[1][i] == '-' || cmd_param[1][i] == '+')
		i++;
	while (cmd_param[1][i])
	{
		if (!ft_isdigit(cmd_param[1][i]))
			return (print_error("minishell: exit: "), print_error(cmd_param[1]),
				cleanup(2, " numeric argument required\n", 2, 2));
		i++;
	}
	if (cmd_param[2])
		return (cleanup(1, "minishell: exit: too many arguments\n", NO_EXIT,
				2));
	if (exit_value < 0 || exit_value > 255)
		exit_value = exit_value % 256;
	return (cleanup(exit_value, "exit\n", exit_value, 1));
}

//+ EXIT STATUS OKOK
//? Calcul de golmon si depasse 255 //
// int	ft_exit(char **cmd_param)
// {
// 	int	i;
// 	int	j;

// 	i = 1;
// 	while (cmd_param[i])
// 	{
// 		j = 0;
// 		while (cmd_param[i][j])
// 		{
// 			if (!ft_isdigit(cmd_param[i][j]))
// 				return (cleanup(2, "exit: numeric argument required\n", 255,
// 						2)); //! pas 255 mais 2 ?
// 			j++;
// 		}
// 		i++;
// 	}
// 	if (i > 2)
// 		return (cleanup(1, "exit: too many arguments\n", NO_EXIT, 2));
// 	if (i == 1)
// 		return (cleanup(0, "exit\n", 0, 1));
// 	return (cleanup(ft_atoi(cmd_param[1]), "exit\n", ft_atoi(cmd_param[1]), 1));
// }

//! cas chelou avec "exit 0-9 "
//! avec un long