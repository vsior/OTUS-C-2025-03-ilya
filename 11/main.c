#include <gtk/gtk.h>
#include <glib/gstdio.h>
#include <sys/stat.h>

#define MAX_PATH_LEN 1000

enum {
   TYPE_COLUMN,
   NAME_COLUMN,
   N_COLUMNS
};

static void activate(GtkApplication *app, gpointer user_data);
static void draw_tree(GtkWidget *tree);
void list_dirs(GtkTreeStore *store, GtkTreeIter *iter, gchar *dirname);

int main(int argc, char **argv) {
    GtkApplication *app;
    int status;

    app = gtk_application_new("org.gtk.example", G_APPLICATION_DEFAULT_FLAGS);
    g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
    status = g_application_run(G_APPLICATION(app), argc, argv);
    g_object_unref(app);

    return status;
}

static void activate(GtkApplication *app, gpointer user_data) {
    GtkWidget *window;
    
    GtkWidget *tree;
    GtkTreeStore *store;
    store = gtk_tree_store_new(N_COLUMNS,
                                G_TYPE_STRING,
                                G_TYPE_STRING);

    window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(window), "Window");
    gtk_window_set_default_size(GTK_WINDOW(window), 600, 600);

    list_dirs(store, NULL, g_get_current_dir());

    tree = gtk_tree_view_new_with_model(GTK_TREE_MODEL(store));

    g_object_unref(G_OBJECT(store));

    draw_tree(tree);
    gtk_window_set_child(GTK_WINDOW(window), tree);

    gtk_window_present(GTK_WINDOW(window));
}

static void draw_tree(GtkWidget *tree) {
   GtkTreeViewColumn *column;
   GtkCellRenderer *renderer;

   renderer = gtk_cell_renderer_text_new();

   column = gtk_tree_view_column_new_with_attributes("Type", renderer, "text", TYPE_COLUMN, NULL);
   gtk_tree_view_append_column(GTK_TREE_VIEW(tree), column);

   renderer = gtk_cell_renderer_text_new();
   column = gtk_tree_view_column_new_with_attributes("Name", renderer, "text", NAME_COLUMN, NULL);
   gtk_tree_view_append_column(GTK_TREE_VIEW(tree), column);

}

void list_dirs(GtkTreeStore *store, GtkTreeIter *iter, gchar *dirname) {
	GDir* dir;
	const gchar* name;
    gchar full_dir_name[MAX_PATH_LEN];

	dir = g_dir_open(dirname, 0, NULL);
	if (dir == NULL)
		return;

	while ((name = g_dir_read_name(dir)) != NULL)
	{
        g_sprintf(full_dir_name, "%s/%s", dirname, name);

        struct stat path_stat;
        stat(full_dir_name, &path_stat);

        GtkTreeIter iter_child;

        gtk_tree_store_append (store, &iter_child, iter);
        gtk_tree_store_set (store, &iter_child,
                            TYPE_COLUMN, (S_ISDIR(path_stat.st_mode)) ? "d" : ".",
                            NAME_COLUMN, full_dir_name,
                            -1);

        if (S_ISDIR(path_stat.st_mode))
            list_dirs(store, &iter_child, full_dir_name);
	}
	g_dir_close(dir);
}