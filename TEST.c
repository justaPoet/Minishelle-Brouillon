#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#include <linux/limits.h>

# include <readline/history.h>       // add_history
# include <readline/readline.h>      // readline

# define SUCCESS 0
# define ERROR 1
# define FAILURE -1
// PATH_MAX 4096

typedef enum s_bool
{
	false,
	true
}					t_bool;

typedef struct				s_env
{
    char            *var;
    char            *val;

	char			*line;
	struct s_env	*next;
	struct s_env	*prev;

}					t_env;

typedef struct s_data
{
    // char        **v_env;
    t_env       *envp;
	char        *username;
}					t_data;

struct				s_token
{
	char			*value;
	int				type;
	struct s_token	*next;
	struct s_token	*prev;

}					t_token;

static void	ft_init_vars(size_t *i, size_t *j, int *start)
{
	*i = 0;
	*j = 0;
	*start = 0;
}

static void	*split_free_tabtab(char **result, int count)
{
	int	i;

	i = 0;
	while (i < count)
	{
		free(result[i]);
		i++;
	}
	free(result);
	return (NULL);
}

static size_t	count_words(const char *str, char separator)
{
	size_t	i;
	size_t	counter;

	i = 0;
	counter = 0;
	while (str[i] != '\0')
	{
		if (str[i] != separator)
		{
			counter++;
			while (str[i] != separator && str[i] != '\0')
				i++;
		}
		else
			while (str[i] == separator && str[i] != '\0')
				i++;
	}
	return (counter);
}

static char	*set_word(const char *str, int start, int end)
{
	char	*word;
	int		i;

	i = 0;
	word = malloc((end - start + 1) * sizeof(char));
	while (start < end)
	{
		word[i] = str[start];
		start++;
		i++;
	}
	word[i] = '\0';
	return (word);
}

char	**ft_split(const char *str, char audd)
{
	size_t	i;
	size_t	j;
	int		start;
	char	**result;

	ft_init_vars(&i, &j, &start);
	result = calloc(count_words(str, audd) + 1, sizeof(char *));
	if (!result)
		return (NULL);
	while (str[i] != '\0')
	{
		if (str[i] == audd)
			i++;
		else
		{
			start = i;
			while (str[i] != audd && str[i] != '\0')
				i++;
			result[j++] = set_word(str, start, i);
			if (!result[j - 1])
				return (split_free_tabtab(result, j));
		}
	}
	return (result);
}

static void *get_data()
{
    static t_data data;
    return (&data);
}

void *get_env()
{
    static t_env env;
    return (&env);
}

char	*ft_strcpy(char *dst, const char *src)
{
	int		i;

	i = -1;
	while (src[++i])
		dst[i] = src[i];
	dst[i] = '\0';
	return (dst);
}

int ft_strlen(const char *s)
{
    int i;

    i = 0;
    while (s[i])
        i++;
    return (i);
}

char	*ft_strdup(char *src)
{
	char	*str;
	int		i;

	str = malloc(sizeof(char) * (ft_strlen(src) + 1));
	if (!str)
		return (NULL);
	i = 0;
	while (src[i])
	{
		str[i] = src[i];
		i++;
	}
	str[i] = '\0';
	return (str);
}

char	*ft_strjoin(char const *s1, char const *s2)
{
	char	*str;
	int		i;
	int		j;

	i = 0;
	j = 0;
	if (s1 == NULL || s2 == NULL)
		return (NULL);
	str = (char *)malloc(ft_strlen(s1) + ft_strlen(s2) + 1);
	if (!str)
		return (NULL);
	while (s1[i])
	{
		str[i] = s1[i];
		i++;
	}
	while (s2[j])
	{
		str[i] = s2[j];
		i++;
		j++;
	}
	str[i] = 0;
	return (str);
}

int	ft_strncmp(const char *s1, const char *s2, size_t n)
{
	size_t	i;

	i = 0;
	while (i < n && (s1[i] || s2[i]))
	{
		if ((unsigned char)s1[i] != (unsigned char)s2[i])
		{
			return ((int)((unsigned char)s1[i] - (unsigned char)s2[i]));
		}
		i++;
	}
	return (0);
}

int	ft_strcmp(char *s1, char *s2)
{
	int	i;

	i = 0;
	while (s1[i] != '\0' || s2[i] != '\0')
	{
		if (s1[i] != s2[i])
		{
			return (s1[i] - s2[i]);
		}
		i++;
	}
	return (0);
}

// ===============================================================================

//! pas teste
// int count_args(t_token *token)
// {
// 	int	i;

// 	i = 0;
// 	if (token == NULL)
// 		return (ERROR);
// 	while (token != NULL)
// 	{
//         if (token->type != 7)
//             return (i - 1);
// 		token = token->next;
//         i++;
// 	}
// 	return (i);
// }

t_env	*find_last_node(t_env *node)
{
	if (node == NULL)
		return (NULL);
	while (node->next)
		node = node->next;
	return (node);
}

void append_node_envp(t_env **envp, char *line)
{
    t_env *new_node;
    t_env *last_node;

    if (!envp)
        return;
    new_node = malloc(sizeof(t_env));
    if (!new_node)
        return;
    new_node->next = NULL;
    new_node->line = line;
    if (*envp == NULL)
    {
        new_node->prev = NULL;
        *envp = new_node;
    }
    else
    {
        last_node = find_last_node(*envp);
        last_node->next = new_node;
        new_node->prev = last_node;
    }
    // printf("APPENDED=%s\n\n", line);
}

void make_env(t_data *data, char **env)
{
    int i = 0;
    
    data->envp = NULL;
    while (env[i])
    {
        append_node_envp(&data->envp, env[i]);
        i++;
    }
}

//! pas d'ordre de prevu avec export
//! prevoir le cas si env -i
int ft_env()
{
    t_data *data = get_data();
    t_env *tmp;

    if (!data || !data->envp)  // Vérifier que data et data->envp existent
        return (ERROR);
    tmp = data->envp;
    while(tmp != NULL)
    {
        printf("%s\n", tmp->line);
        tmp = tmp->next;
    }
    return (SUCCESS);
}

// possibilite de trouver current path avec getenv() mais bon
//! exec pwd peu importe quelle/combien d'argument le suit
int ft_pwd()
{
    char cwd[PATH_MAX];

    if (getcwd(cwd, PATH_MAX)) //! cherche direct dans l'env 
    {
        printf("%s\n", cwd);
        return (SUCCESS);
    }
    else
    {
        perror("Failure: getcwd\n");
        return (FAILURE);
    }
}

//! 1/ var doit contenir le = pour bien marcher
//! 2/ cas "_"
char *search_env_var(char *var)
{
    int len;
    char *value;
    t_env *tmp;
    t_data *data;

    if (!var)
        return (NULL);
    data = get_data();
    if (!data || !data->envp)
        return (NULL);
    len = ft_strlen(var);
    tmp = data->envp;
    while (tmp != NULL)
    {
        if (ft_strncmp(var, tmp->line, len) == 0 && tmp->line[len] == '=')
        {
            value = malloc(ft_strlen(tmp->line + len + 1) + 1);
            if (!value)
                return (NULL);
            ft_strcpy(value, tmp->line + len + 1);
            return (value);
        }
        tmp = tmp->next;
    }
    return (NULL);
}

// changer PWD dans l'env
//! BESOIN DE CHANGE PWD ET OLDPWD (et donc export) POUR CHEMEIN ABSOLUT ET RELATIF  
int change_cd(char *direction)
{
    int audd;

    char *res;
    t_data *data = get_data(); 
    if (ft_strcmp(direction, "") == 0 || ft_strcmp(direction, "~") == 0 || ft_strcmp(direction, "--") == 0)
    {
        res = search_env_var("HOME");
        if (!res)
            return (FAILURE);
        chdir(res);
        free(res);
        return (SUCCESS);
    }
    else 
        audd = chdir(direction);
    if (audd == -1)
    {    
        // char *new_direction = ft_strjoin(getenv("PWD"), direction); //! BESOIN DE CHANGE PWD ET OLDPWD (et donc export) POUR CHEMEIN ABSOLUT ET RELATIF  
        // printf("=======%s\n", new_direction);
        // audd = chdir(new_direction);
        // free(new_direction);
        if (audd != -1)
            return (SUCCESS);
        printf("Error: cd: %s: No such file or directory\n", direction);
        return (ERROR);
    }
    return (SUCCESS);
}

// with only a relative or absolute path 
int ft_cd(char **line)
{
    int exit_status;
    int nb_args;
    nb_args = 2; /*count_args(token);*/

    if (nb_args > 2)
    {    
        printf("Error: cd: Too many arguments\n");
        return (ERROR);
    }
    if (nb_args == 2)
        change_cd(line[1]);
}

//! ======================================================================================
//! ======================================================================================
//! ======================================================================================

void print_line_export(char *line)
{
    int i;
    
    i = 0;
    printf("declare -x ");
    while (line[i] != '=')
    {
        printf("%c", line[i]);
        i++;
    }
    printf("=");
    i++;
    printf("\"");
    while (line[i])
    {
        printf("%c", line[i]);
        i++;
    }
    printf("\"");
    printf("\n");
}

void	ft_free_tabtab(char **tabtab)
{
	int	i;

	i = 0;
	while (tabtab[i])
    {
		free(tabtab[i]);
        i++;
    }
    free(tabtab);
	tabtab = NULL;
}

int sort_envp_and_print(char **envp, int count)
{
    int i;
    int j;
    char *temp;

    i = 0;
    if (!envp || !*envp || count <= 0)
        return (FAILURE);
    while (i < count - 1)
    {
        if (ft_strcmp(envp[i], envp[i + 1]) > 0)
        {
            temp = envp[i];
            envp[i] = envp[i + 1];
            envp[i + 1] = temp;
            i = 0;
        }
        else 
            i++;
    }
    i = 0;
    while (envp[i])
        print_line_export(envp[i++]);
    return (SUCCESS);
}

char **copy_envp_to_tab(t_env *envp, int count)
{
    int i;
    t_env *tmp;
    char **env_tab;
    
    i = 0;
    tmp = envp;
    env_tab = malloc((count + 1) * sizeof(char *));
    if (!env_tab)
        return (NULL);
    while (i < count)
    {
        env_tab[i] = strdup(tmp->line);
        if (!env_tab[i])
        {
            ft_free_tabtab(env_tab);
            return (NULL);
        }
        tmp = tmp->next;
        i++;
    }
    env_tab[count] = NULL; 
    return (env_tab);
}

int count_envp_node(t_env *envp)
{
    t_env *tmp;
    int i;

    i = 0;
    if (!envp || !envp->next)
        return (FAILURE);
    tmp = envp;
    while(tmp != NULL)
    {
        i++;
        tmp = tmp->next;
    }
    return (i);
}

void	del_node_t_env(t_env **envp)
{
	t_env	*del_node;
	t_env	*new_first;

	if (envp == NULL || *envp == NULL)
		return ;
	del_node = *envp;
	new_first = del_node->next;
	if (new_first)
		new_first->prev = NULL;
	*envp = new_first;
	del_node->next = NULL;
	del_node->prev = NULL;
	free(del_node);
}

void	destroy_env(t_env **env)
{
	if (env == NULL || *env == NULL)
		return ;
	while (*env != NULL)
		del_node_t_env(env);
}

char	*join_var_and_val(char const *s1, char const *s2)
{
	char	*str;
	int		i;
	int		j;

	i = 0;
	j = 0;
	if (s1 == NULL || s2 == NULL)
		return (NULL);
	str = (char *)malloc(ft_strlen(s1) + ft_strlen(s2) + 2); //? 1 pour '=' 1 pour '\0'
	if (!str)
		return (NULL);
	while (s1[i])
	{
		str[i] = s1[i];
		i++;
	}
    str[i] = '=';
    i++;
	while (s2[j])
	{
		str[i] = s2[j];
		i++;
		j++;
	}
	str[i] = 0;
	return (str);
}

// premiere lettre du NOM != 0-9
// pas de caracteres speciaux dans la variable
// ne prendre en compte que le premier arg comme la valeur de la nouvelle variable d'env
//! prevoir un cas pour les doublons 
int export_new_var_envp(t_env *envp, char *argument)
{
    int i;
    int j;
    char **res;
    char invalide_char[26] = "!@#$%^&*()+=,.:/?<>{}[]|-"; // OKOK

    i = 0;
    j = 0;
    res = ft_split(argument, '=');
    if (isdigit(res[0][0]))
    {
        printf("Error: export: %s: not a valid identifier\n", res[0]);
        ft_free_tabtab(res); //!
        return (ERROR);
    }
    while (res[0][j])
    {
        while (invalide_char[i])
        {
            if (res[0][j] == invalide_char[i])
            {
                printf("Error: export: %s: not a valid identifier\n", res[0]);
                return (ERROR);
            }
            i++;
        }
        i = 0;
        j++;
    }
    char *to_print = join_var_and_val(res[0], res[1]); //! strjoin trafique avec '='
    append_node_envp(&envp, to_print); // pbl si je passe line(=tmp) jsp pk
    ft_free_tabtab(res);
    // free(to_print);
}

//! cas si envp est supp (// env -i)
int ft_export(t_env *envp, char *tmp)
{
    int i;

    i = 0;
    if (!envp)
        return (FAILURE);

    if (ft_strcmp(tmp, "export") == 0) // export (no args)
    {
        int count = count_envp_node(envp);
        char **tab_envp = copy_envp_to_tab(envp, count);
        sort_envp_and_print(tab_envp, count);
        ft_free_tabtab(tab_envp);
        return (SUCCESS);
    }
    else // export with args
    {
        while (tmp[i])
        {
            if (tmp[i] == '=')
            {
                if (export_new_var_envp(envp, tmp) == SUCCESS) // cas initialisation nvl var
                    return (SUCCESS);
                else 
                    return (ERROR);
            }
            i++;
        }
        // cas ou export var deja existante
    }
}

//* export NAME=value      # Définit et exporte la variable
//* export NAME            # Exporte une variable existante
// export                  # Affiche toutes les variables exportées

//! export 1VAR=value      # Nom invalide (commence par un chiffre)
//! export VAR!=value      # Caractères spéciaux non autorisés dans le nom
//! ... la valeur nsm

//? ATTENTION aux noms et caracteres invalides pour la creation de variables 

//==> creer une nouvelle variable
//? Rajouter une verif pour noms et caracteres invalides...
//? ... puis l'ajouter a la suite des variables d'environnement de envp grace a append_envp()

//==> exporter une variable deja existante
//? 

//! EXPORT ==> UNSET ==> CD (change PWD/OLDPWD)

// int main(int ac, char **av, char **env)
// {
//     // t_data *data = get_data(); 
//     // make_env(data, env);
//     // ft_export(data, av[2]);
//     // destroy_env(&data->envp);

//     int i = 0;
//     char **res = ft_split(av[2], '=');
//     while (res[i])
//     {
//         printf(">%s\n", res[i]);
//         i++;
//     }
// }

int	main(int ac, char **av, char **env)
{
	char *line;
    t_data *data = get_data(); 
    data->username = getenv("USER");

    
    make_env(data, env);

    // char *audd = search_env_var(av[1]);
    // printf("%s\n", audd);
    // free(audd);

	while (1)
	{
		line = readline("minishell>");
        if (ft_strcmp(line, "pwd") == 0)
            ft_pwd();
        else if (ft_strcmp(line, "exit") == 0)
            break ;
        else if (ft_strcmp(line, "env") == 0)
            ft_env();
        // else
        //     change_cd(line);
        else /*if (ft_strcmp(line, "export") == 0)*/
            ft_export(data->envp, line);
        // else 
        //     ft_echo(line);
        add_history(line); // OKOK
        if (line != NULL)
            free(line);
	}
    if (line != NULL)
        free(line);
    rl_clear_history(); // OKOK
    destroy_env(&data->envp);
    return (0);
}

//? Faire une fonction "compil_args()" qui va relier les token allant ensemble dans un char** "cmd_args"...
//? ...pour faciliter l'execution de chaque cmd
//? Prendre en compte le '$' dans certains cas (//echo)





















// int export_no_args()
// {
//     int i;
//     t_data *data = get_data();
//     t_env *tmp;

//     i = 0;
//     if (!data || !data->envp)
//         return (ERROR);
//     tmp = data->envp;
//     while(tmp != NULL)
//     {
//         print_export(tmp->line);
//         tmp = tmp->next;
//     }
//     return (SUCCESS);
// }



// //! 1/ var doit contenir le = pour bien marcher
// //! 2/ cas "_"
// char *search_env_var(char *var)
// {
//     int i;
//     int len;
//     char *value;

//     i = 0;
//     len = 0;
//     t_data *data = get_data();
//     if (!var)
//         return (NULL);
//     len = ft_strlen(var);
//     while (data->v_env[i])
//     {
//         if (ft_strncmp(var, data->v_env[i], len) == 0 && data->v_env[i][len] == '=')
//         {
//             value = (char *)malloc(ft_strlen(data->v_env[i] + len + 1) + 1);
//             if (!value)
//                 return (NULL);
//             ft_strcpy(value, data->v_env[i] + len + 1);
//             return value;
//         }
//         i++;
//     }
//     return (NULL);
// }

// void make_env(t_data *data, char **env)
// {
//     int i;

//     i = 0;
//     // if (!env) 
//     //     remake_env();
//     while(env[i])
//         i++;
//     data->v_env = (char**)malloc(sizeof(char *) * (i + 1));
//     i = 0;
//     while (env[i])
//     {
//         data->v_env[i] = ft_strdup(env[i]);
//         i++;
//     }
//     data->v_env[i] = 0;
// }

// int ft_env()
// {
//     int i;
//     t_data* data = get_data();

//     i = 0;
//     while(data->v_env[i])
//     {
//         printf("%s\n", data->v_env[i]);
//         i++;
//     }
//     return (SUCCESS);
// }




// ==22247== Conditional jump or move depends on uninitialised value(s)
// ==22247==    at 0x410E30: envp_sort_and_print (in /home/apoet/42Temporary/dir-test/a.out)
// ==22247==    by 0x410F57: ft_export (in /home/apoet/42Temporary/dir-test/a.out)
// ==22247==    by 0x411077: main (in /home/apoet/42Temporary/dir-test/a.out)
// ==22247== 
// ==22247== Conditional jump or move depends on uninitialised value(s)
// ==22247==    at 0x410CA0: ft_free_tabtab (in /home/apoet/42Temporary/dir-test/a.out)
// ==22247==    by 0x410F5F: ft_export (in /home/apoet/42Temporary/dir-test/a.out)
// ==22247==    by 0x411077: main (in /home/apoet/42Temporary/dir-test/a.out)
// ==22247== 
// ==22247== 

// int ft_env()
// {
//     int i;
//     t_data* data = get_data();

//     i = 0;
//     while(data->v_env[i])
//     {
//         printf("%s\n", data->v_env[i]);
//         i++;
//     }
//     return (SUCCESS);
// }