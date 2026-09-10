#include <gtk/gtk.h>
#include <stdio.h>

typedef struct {
	GtkWidget *window;
	GtkWidget *list;
	char *config_file;
} Selector;

static void save_games(Selector *selector)
{
	GString *contents = g_string_new(NULL);
	GList *rows = gtk_container_get_children(GTK_CONTAINER(selector->list));
	for (GList *item = rows; item; item = item->next)
	{
		const char *uri = g_object_get_data(G_OBJECT(item->data), "game-uri");
		if (uri) g_string_append_printf(contents, "%s\n", uri);
	}
	g_list_free(rows);

	char *directory = g_path_get_dirname(selector->config_file);
	g_mkdir_with_parents(directory, 0700);
	g_file_set_contents(selector->config_file, contents->str, contents->len, NULL);
	g_free(directory);
	g_string_free(contents, TRUE);
}

static gboolean uri_is_listed(Selector *selector, const char *uri)
{
	GList *rows = gtk_container_get_children(GTK_CONTAINER(selector->list));
	for (GList *item = rows; item; item = item->next)
	{
		const char *existing = g_object_get_data(G_OBJECT(item->data), "game-uri");
		if (g_strcmp0(existing, uri) == 0)
		{
			g_list_free(rows);
			return TRUE;
		}
	}
	g_list_free(rows);
	return FALSE;
}

static void add_uri(Selector *selector, const char *uri)
{
	if (!uri || uri_is_listed(selector, uri)) return;

	GFile *folder = g_file_new_for_uri(uri);
	char *name = g_file_get_basename(folder);
	char *path = g_file_get_parse_name(folder);
	char *label_text = g_strdup_printf("%s\n%s", name ? name : "Game", path);
	GtkWidget *label = gtk_label_new(label_text);
	gtk_label_set_xalign(GTK_LABEL(label), 0.0f);
	gtk_label_set_ellipsize(GTK_LABEL(label), PANGO_ELLIPSIZE_MIDDLE);
	GtkWidget *row = gtk_list_box_row_new();
	gtk_container_add(GTK_CONTAINER(row), label);
	g_object_set_data_full(G_OBJECT(row), "game-uri", g_strdup(uri), g_free);
	gtk_widget_show_all(row);
	gtk_container_add(GTK_CONTAINER(selector->list), row);
	gtk_list_box_select_row(GTK_LIST_BOX(selector->list), GTK_LIST_BOX_ROW(row));

	g_free(label_text);
	g_free(path);
	g_free(name);
	g_object_unref(folder);
}

static void folder_chosen(GtkNativeDialog *dialog, gint response, gpointer data)
{
	Selector *selector = data;
	if (response == GTK_RESPONSE_ACCEPT)
	{
		GFile *folder = gtk_file_chooser_get_file(GTK_FILE_CHOOSER(dialog));
		char *uri = g_file_get_uri(folder);
		add_uri(selector, uri);
		save_games(selector);
		g_free(uri);
		g_object_unref(folder);
	}
	g_object_unref(dialog);
}

static void add_clicked(GtkButton *button, gpointer data)
{
	Selector *selector = data;
	GtkFileChooserNative *chooser = gtk_file_chooser_native_new(
		"Add an Aleph One game-data folder", GTK_WINDOW(selector->window),
		GTK_FILE_CHOOSER_ACTION_SELECT_FOLDER, "Add Game", "Cancel");
	g_signal_connect(chooser, "response", G_CALLBACK(folder_chosen), selector);
	gtk_native_dialog_show(GTK_NATIVE_DIALOG(chooser));
}

static void remove_clicked(GtkButton *button, gpointer data)
{
	Selector *selector = data;
	GtkListBoxRow *row = gtk_list_box_get_selected_row(GTK_LIST_BOX(selector->list));
	if (!row) return;
	gtk_widget_destroy(GTK_WIDGET(row));
	save_games(selector);
}

static void row_activated(GtkListBox *box, GtkListBoxRow *row, gpointer data)
{
	gtk_dialog_response(GTK_DIALOG(((Selector *)data)->window), GTK_RESPONSE_OK);
}

static void load_games(Selector *selector)
{
	char *contents = NULL;
	if (!g_file_get_contents(selector->config_file, &contents, NULL, NULL)) return;
	char **lines = g_strsplit(contents, "\n", -1);
	for (char **line = lines; *line; ++line)
		if (**line) add_uri(selector, *line);
	g_strfreev(lines);
	g_free(contents);
}

int main(int argc, char **argv)
{
	gtk_init(&argc, &argv);
	Selector selector = {0};
	selector.config_file = g_build_filename(g_get_user_config_dir(),
		"sprintathon", "games", NULL);
	selector.window = gtk_dialog_new_with_buttons("Sprintathon — Choose Game", NULL,
		GTK_DIALOG_MODAL, "Quit", GTK_RESPONSE_CANCEL,
		"Play", GTK_RESPONSE_OK, NULL);
	gtk_window_set_default_size(GTK_WINDOW(selector.window), 620, 420);
	GtkWidget *quit_button = gtk_dialog_get_widget_for_response(
		GTK_DIALOG(selector.window), GTK_RESPONSE_CANCEL);
	GtkWidget *play_button = gtk_dialog_get_widget_for_response(
		GTK_DIALOG(selector.window), GTK_RESPONSE_OK);
	gtk_widget_set_size_request(quit_button, 150, 56);
	gtk_widget_set_size_request(play_button, 150, 56);
	gtk_widget_set_margin_start(quit_button, 8);
	gtk_widget_set_margin_end(play_button, 8);
	gtk_dialog_set_default_response(GTK_DIALOG(selector.window), GTK_RESPONSE_OK);

	GtkWidget *content = gtk_dialog_get_content_area(GTK_DIALOG(selector.window));
	GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
	gtk_container_set_border_width(GTK_CONTAINER(box), 16);
	gtk_container_add(GTK_CONTAINER(content), box);
	GtkWidget *heading = gtk_label_new(NULL);
	gtk_label_set_markup(GTK_LABEL(heading),
		"Choose a game to play or add a new folder containing Marathon game files.\n"
		"You can use the Windows game folders for the original Bungie games, which are free to download at "
		"<a href=\"https://alephone.lhowon.org/\">alephone.lhowon.org</a>.\n"
		"Third-party scenarios also work with Sprintathon.");
	gtk_label_set_xalign(GTK_LABEL(heading), 0.0f);
	gtk_label_set_line_wrap(GTK_LABEL(heading), TRUE);
	gtk_label_set_selectable(GTK_LABEL(heading), TRUE);
	gtk_box_pack_start(GTK_BOX(box), heading, FALSE, FALSE, 0);

	GtkWidget *scroll = gtk_scrolled_window_new(NULL, NULL);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scroll),
		GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);
	gtk_scrolled_window_set_min_content_height(GTK_SCROLLED_WINDOW(scroll), 280);
	gtk_widget_set_vexpand(scroll, TRUE);
	gtk_box_pack_start(GTK_BOX(box), scroll, TRUE, TRUE, 0);
	selector.list = gtk_list_box_new();
	gtk_list_box_set_selection_mode(GTK_LIST_BOX(selector.list), GTK_SELECTION_SINGLE);
	gtk_container_add(GTK_CONTAINER(scroll), selector.list);
	g_signal_connect(selector.list, "row-activated", G_CALLBACK(row_activated), &selector);

	GtkWidget *buttons = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8);
	GtkWidget *add = gtk_button_new_with_label("Add Game Folder…");
	GtkWidget *remove = gtk_button_new_with_label("Remove");
	gtk_box_pack_start(GTK_BOX(buttons), add, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(buttons), remove, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(box), buttons, FALSE, FALSE, 0);
	g_signal_connect(add, "clicked", G_CALLBACK(add_clicked), &selector);
	g_signal_connect(remove, "clicked", G_CALLBACK(remove_clicked), &selector);

	load_games(&selector);
	gtk_widget_show_all(selector.window);
	int response = gtk_dialog_run(GTK_DIALOG(selector.window));
	int status = 1;
	if (response == GTK_RESPONSE_OK)
	{
		GtkListBoxRow *row = gtk_list_box_get_selected_row(GTK_LIST_BOX(selector.list));
		if (row)
		{
			const char *uri = g_object_get_data(G_OBJECT(row), "game-uri");
			GFile *folder = g_file_new_for_uri(uri);
			char *path = g_file_get_path(folder);
			if (path) { puts(path); status = 0; }
			g_free(path);
			g_object_unref(folder);
		}
	}

	gtk_widget_destroy(selector.window);
	g_free(selector.config_file);
	return status;
}
