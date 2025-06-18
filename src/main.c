// src/main.c
// Program C dengan GUI GTK 4, dengan menu instalasi dan manajer proyek yang fungsional.

#include <gtk/gtk.h>
#include <stdlib.h> // Diperlukan untuk fungsi system()
#include <stdio.h>  // Diperlukan untuk snprintf()
#include <dirent.h> // Diperlukan untuk memindai direktori
#include <string.h> // Diperlukan untuk strcmp()
#include <glib.h>   // Diperlukan untuk g_get_user_name()
#include <unistd.h> // Diperlukan untuk W_OK dalam g_access()

// MODIFIKASI: Mendefinisikan path skrip secara global untuk akses yang mudah
static const char *xschem_path = "/usr/local/share/itsopen/script/its-xschem-install.sh";
static const char *ngspice_path = "/usr/local/share/itsopen/script/its-ngspice-install.sh";
static const char *magic_path = "/usr/local/share/itsopen/script/its-magic-install.sh";
static const char *netgen_path = "/usr/local/share/itsopen/script/its-netgen-install.sh";
static const char *gf180mcu_path = "/usr/local/share/itsopen/script/its-gf180mcu-install.sh";
static const char *sky130_path = "/usr/local/share/itsopen/script/its-sky130-install.sh";
static const char *lvs_script_path = "/usr/local/share/itsopen/script/iic-lvs.sh"; // Path untuk skrip LVS
static const char *drc_script_path = "/usr/local/share/itsopen/script/iic-drc.sh"; // Path untuk skrip DRC
char *pdk_value = "gf180mcuD"; // Nilai PDK default, bisa diubah sesuai kebutuhan

// --- Data Structures ---
typedef struct {
    GtkWidget *parent_dialog;
    GtkWidget *project_name_entry;
    GtkWidget *location_button;
    GtkWidget *pdk_selection_dropdown; // Dropdown PDK untuk jendela Buat Proyek
} CreateProjectData;

typedef struct {
    GtkWidget *window;
    GtkWidget *current_project_label;
    GtkWidget *project_content_grid; // Grid untuk menampung semua konten proyek
    GtkWidget *pdk_selection_dropdown; // Dropdown untuk PDK
    GtkWidget *schematic_name_entry;
    GtkWidget *layout_name_entry;
    GtkWidget *lvs_schematic_entry;  // Entry untuk LVS
    GtkWidget *lvs_layout_entry;      // Entry untuk LVS
    GtkWidget *drc_layout_entry;      // Entry untuk DRC
    GtkWidget *schematic_list_box; // Box untuk menampung tombol file skematik
    GtkWidget *layout_list_box;     // Box untuk menampung tombol file layout
} ProjectManagerData;


// --- Fungsi Callback ---

// Callback untuk menampilkan dialog "About"
static void show_about_dialog(GtkWidget *widget, gpointer parent_window) {
    const char *authors[] = {"M Taufiqul Huda | E-mail : muhammadhuda1025@gmail.com", NULL};
    gtk_show_about_dialog(GTK_WINDOW(parent_window),
                          "program-name", "Installer Suite ITS Open-Source IC",
                          "version", "1.22",
                          "comments", "An installer application and project manager tool for open-source Electronic Design Automation (EDA) tools.\nAll tools from http://opencircuitdesign.com/index.html and PDKs from SkyWater and GF180MCU.",
                          "website", "https://www.its.ac.id",
                          "copyright", "© 2025 Institut Teknologi Sepuluh Nopember (ITS) Surabaya, Indonesia",
                          "authors", authors,
                          "logo-icon-name", "utilities-terminal",
                          NULL);
}

// Callback untuk menjalankan skrip instalasi
static void run_script_callback(GtkWidget *widget, gpointer data) {
    const char *script_path = (const char *)data;
    char full_command[1024];

    g_print("\n--- Button '%s' Clicked! ---\n", gtk_button_get_label(GTK_BUTTON(widget)));
    snprintf(full_command, sizeof(full_command), "%s", script_path);
    g_print("Execute the command: %s\n", full_command);
    
    int result = system(full_command);

    if (result == 0) {
        g_print("--- Script Success Executed! ---\n");
    } else {
        g_print("--- Script Failed Executed. ---\n");
    }
}


// --- Jendela Instalasi ---
static void show_installer_window(GtkWidget *widget, gpointer parent_window) {
    GtkWidget *window = gtk_window_new();
    gtk_window_set_title(GTK_WINDOW(window), "Installation Manager");
    gtk_window_set_transient_for(GTK_WINDOW(window), GTK_WINDOW(parent_window));
    gtk_window_set_modal(GTK_WINDOW(window), TRUE);
    gtk_window_set_default_size(GTK_WINDOW(window), 400, -1);
    gtk_window_set_resizable(GTK_WINDOW(window), TRUE);

    GtkWidget *main_box, *packages_label, *pdk_label, *separator1;
    GtkWidget *button_xschem, *button_ngspice, *button_magic, *button_netgen, *button_gf180mcu, *button_sky130;

    main_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 15);
    gtk_widget_set_margin_top(main_box, 20);
    gtk_widget_set_margin_bottom(main_box, 20);
    gtk_widget_set_margin_start(main_box, 20);
    gtk_widget_set_margin_end(main_box, 20);
    gtk_window_set_child(GTK_WINDOW(window), main_box);

    packages_label = gtk_label_new("Chose the package for install:");
    gtk_widget_set_halign(packages_label, GTK_ALIGN_START);
    gtk_box_append(GTK_BOX(main_box), packages_label);
    button_xschem = gtk_button_new_with_label("Install Xschem");
    button_ngspice = gtk_button_new_with_label("Install Ngspice");
    button_magic = gtk_button_new_with_label("Install Magic");
    button_netgen = gtk_button_new_with_label("Install Netgen");
    gtk_box_append(GTK_BOX(main_box), button_xschem);
    gtk_box_append(GTK_BOX(main_box), button_ngspice);
    gtk_box_append(GTK_BOX(main_box), button_magic);
    gtk_box_append(GTK_BOX(main_box), button_netgen);

    separator1 = gtk_separator_new(GTK_ORIENTATION_HORIZONTAL);
    gtk_box_append(GTK_BOX(main_box), separator1);

    pdk_label = gtk_label_new("Chose the PDK for install:");
    gtk_widget_set_halign(pdk_label, GTK_ALIGN_START);
    gtk_box_append(GTK_BOX(main_box), pdk_label);
    button_gf180mcu = gtk_button_new_with_label("Install GF180MCU");
    button_sky130 = gtk_button_new_with_label("Install Sky130 PDK");
    gtk_box_append(GTK_BOX(main_box), button_gf180mcu);
    gtk_box_append(GTK_BOX(main_box), button_sky130);

    g_signal_connect(button_xschem, "clicked", G_CALLBACK(run_script_callback), (gpointer)xschem_path);
    g_signal_connect(button_ngspice, "clicked", G_CALLBACK(run_script_callback), (gpointer)ngspice_path);
    g_signal_connect(button_magic, "clicked", G_CALLBACK(run_script_callback), (gpointer)magic_path);
    g_signal_connect(button_netgen, "clicked", G_CALLBACK(run_script_callback), (gpointer)netgen_path);
    g_signal_connect(button_gf180mcu, "clicked", G_CALLBACK(run_script_callback), (gpointer)gf180mcu_path);
    g_signal_connect(button_sky130, "clicked", G_CALLBACK(run_script_callback), (gpointer)sky130_path);

    gtk_window_present(GTK_WINDOW(window));
}


// --- Jendela Create Project (Bagian dari Project Manager) ---
static void select_project_location_response(GtkFileChooserDialog *dialog, int response, gpointer user_data) {
    GtkButton *location_button = GTK_BUTTON(user_data);
    if (response == GTK_RESPONSE_ACCEPT) {
        GFile *folder = gtk_file_chooser_get_file(GTK_FILE_CHOOSER(dialog));
        char *folder_path = g_file_get_path(folder);
        g_object_set_data(G_OBJECT(location_button), "selected-path", g_strdup(folder_path));
        gtk_button_set_label(location_button, folder_path);
        g_free(folder_path);
        g_object_unref(folder);
    }
    gtk_window_destroy(GTK_WINDOW(dialog));
}

static void on_select_location_clicked(GtkWidget *widget, gpointer parent_dialog) {
    GtkWidget *dialog = gtk_file_chooser_dialog_new("Choose Project Location",
                                                    GTK_WINDOW(parent_dialog),
                                                    GTK_FILE_CHOOSER_ACTION_SELECT_FOLDER,
                                                    "_Cancel", GTK_RESPONSE_CANCEL,
                                                    "_OK", GTK_RESPONSE_ACCEPT,
                                                    NULL);
    g_signal_connect(dialog, "response", G_CALLBACK(select_project_location_response), widget);
    gtk_window_present(GTK_WINDOW(dialog));
}

static void on_create_project_confirmed(GtkWidget *widget, gpointer data) {
    CreateProjectData *project_data = (CreateProjectData *)data;
    const char *project_name = gtk_editable_get_text(GTK_EDITABLE(project_data->project_name_entry));
    const char *location_path = (const char*)g_object_get_data(G_OBJECT(project_data->location_button), "selected-path");
    
    GtkDropDown* pdk_dropdown = GTK_DROP_DOWN(project_data->pdk_selection_dropdown);
    const char* selected_pdk_name = gtk_string_list_get_string(GTK_STRING_LIST(gtk_drop_down_get_model(pdk_dropdown)), gtk_drop_down_get_selected(pdk_dropdown));


    GtkWidget *message_dialog;
    if (project_name == NULL || *project_name == '\0' || location_path == NULL) {
        message_dialog = gtk_message_dialog_new(GTK_WINDOW(project_data->parent_dialog),
                                                GTK_DIALOG_DESTROY_WITH_PARENT | GTK_DIALOG_MODAL,
                                                GTK_MESSAGE_ERROR, GTK_BUTTONS_OK,
                                                "Project name and location cannot be empty!");
    } else if (selected_pdk_name == NULL) {
         message_dialog = gtk_message_dialog_new(GTK_WINDOW(project_data->parent_dialog),
                                                GTK_DIALOG_DESTROY_WITH_PARENT | GTK_DIALOG_MODAL,
                                                GTK_MESSAGE_ERROR, GTK_BUTTONS_OK,
                                                "Choose a PDK first from the dropdown menu!");
    }
    else {
        if (strcmp(selected_pdk_name, "GF180MCU") == 0) {
            pdk_value = "gf180mcuD";
        } else if (strcmp(selected_pdk_name, "Sky130") == 0) {
            pdk_value = "sky130A";
        }

        char project_root[2048];
        snprintf(project_root, sizeof(project_root), "%s/%s", location_path, project_name);

        char command[8192];
        snprintf(command, sizeof(command), 
                 "mkdir -p \"%s/xschem\" \"%s/magic\" \"%s/lvs\" \"%s/drc\" && "
                 "ln -s /usr/local/share/pdk/%s/libs.tech/xschem/xschemrc \"%s/xschemrc\" && "
                 "ln -s /usr/local/share/pdk/%s/libs.tech/magic/gf180mcuD.magicrc \"%s/.magicrc\"",
                 project_root, project_root, project_root, project_root, 
                 pdk_value, project_root, 
                 pdk_value, project_root);

        g_print("Create Project Here: %s\n", project_root);
        g_print("Execute command: %s\n", command);
        int result = system(command);

        if (result == 0) {
            message_dialog = gtk_message_dialog_new(GTK_WINDOW(project_data->parent_dialog),
                                                    GTK_DIALOG_DESTROY_WITH_PARENT | GTK_DIALOG_MODAL,
                                                    GTK_MESSAGE_INFO, GTK_BUTTONS_OK,
                                                    "Project directory '%s' with PDK %s created.", project_root, selected_pdk_name);
             if (result == 0) gtk_window_destroy(GTK_WINDOW(project_data->parent_dialog));
        } else {
            message_dialog = gtk_message_dialog_new(GTK_WINDOW(project_data->parent_dialog),
                                                    GTK_DIALOG_DESTROY_WITH_PARENT | GTK_DIALOG_MODAL,
                                                    GTK_MESSAGE_ERROR, GTK_BUTTONS_OK,
                                                    "Failed to create project directory.");
        }
    }

    g_signal_connect_swapped(message_dialog, "response", G_CALLBACK(gtk_window_destroy), message_dialog);
    gtk_window_present(GTK_WINDOW(message_dialog));
}

static void show_create_project_window(GtkWidget *widget, gpointer parent_window) {
    GtkWidget *dialog = gtk_window_new();
    gtk_window_set_title(GTK_WINDOW(dialog), "Create New Project");
    gtk_window_set_transient_for(GTK_WINDOW(dialog), GTK_WINDOW(parent_window));
    gtk_window_set_modal(GTK_WINDOW(dialog), TRUE);
    gtk_window_set_resizable(GTK_WINDOW(dialog), TRUE);
    gtk_window_set_default_size(GTK_WINDOW(dialog), 720, -1);

    GtkWidget *grid = gtk_grid_new();
    gtk_widget_set_margin_top(grid, 12);
    gtk_widget_set_margin_bottom(grid, 12);
    gtk_widget_set_margin_start(grid, 12);
    gtk_widget_set_margin_end(grid, 12);
    gtk_grid_set_row_spacing(GTK_GRID(grid), 12);
    gtk_grid_set_column_spacing(GTK_GRID(grid), 12);
    gtk_window_set_child(GTK_WINDOW(dialog), grid);

    // --- Nama Proyek ---
    GtkWidget *name_label = gtk_label_new("Project Name:");
    gtk_widget_set_halign(name_label, GTK_ALIGN_START);
    GtkWidget *name_entry = gtk_entry_new();
    gtk_widget_set_hexpand(name_entry, TRUE);
    gtk_grid_attach(GTK_GRID(grid), name_label, 0, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), name_entry, 1, 0, 1, 1);

    // --- Lokasi Proyek ---
    GtkWidget *location_label = gtk_label_new("Directory:");
    gtk_widget_set_halign(location_label, GTK_ALIGN_START);
    GtkWidget *location_button = gtk_button_new_with_label("Choose Directory...");
    gtk_widget_set_hexpand(location_button, TRUE);
    gtk_grid_attach(GTK_GRID(grid), location_label, 0, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), location_button, 1, 1, 1, 1);
    g_signal_connect(location_button, "clicked", G_CALLBACK(on_select_location_clicked), dialog);

    // --- Pilihan PDK ---
    GtkWidget *pdk_label = gtk_label_new("Choose PDK:");
    gtk_widget_set_halign(pdk_label, GTK_ALIGN_START);
    const char *pdk_items[] = {"GF180MCU", "Sky130", NULL};
    GtkStringList *pdk_string_list = gtk_string_list_new(pdk_items);
    GtkWidget *pdk_dropdown = gtk_drop_down_new(G_LIST_MODEL(pdk_string_list), NULL);
    gtk_widget_set_hexpand(pdk_dropdown, TRUE);
    gtk_grid_attach(GTK_GRID(grid), pdk_label, 0, 2, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), pdk_dropdown, 1, 2, 1, 1);


    // --- Tombol Aksi ---
    GtkWidget *action_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 12);
    gtk_widget_set_halign(action_box, GTK_ALIGN_END);
    GtkWidget *cancel_button = gtk_button_new_with_label("Cancel");
    GtkWidget *create_button = gtk_button_new_with_label("Create Project");
    gtk_widget_add_css_class(create_button, "suggested-action");
    gtk_box_append(GTK_BOX(action_box), cancel_button);
    gtk_box_append(GTK_BOX(action_box), create_button);
    gtk_grid_attach(GTK_GRID(grid), action_box, 1, 3, 1, 1);

    CreateProjectData *project_data = g_new(CreateProjectData, 1);
    project_data->parent_dialog = dialog;
    project_data->project_name_entry = name_entry;
    project_data->location_button = location_button;
    project_data->pdk_selection_dropdown = pdk_dropdown;

    g_signal_connect_swapped(cancel_button, "clicked", G_CALLBACK(gtk_window_destroy), dialog);
    g_signal_connect(create_button, "clicked", G_CALLBACK(on_create_project_confirmed), project_data);
    g_signal_connect_swapped(dialog, "destroy", G_CALLBACK(g_free), project_data);
    
    gtk_window_present(GTK_WINDOW(dialog));
}


// --- Jendela Project Manager ---
void on_refresh_clicked(GtkWidget *widget, gpointer data); // Forward declaration

// Generic file opener callback
static void on_design_file_clicked(GtkWidget *widget, gpointer data) {
    const char *file_path = (const char *)data;
    char command[8192];
    
    if (g_str_has_suffix(file_path, ".sch")) {
        snprintf(command, sizeof(command), "(cd \"$(dirname \"%s\")\" && export PDK=%s && xschem --rcfile $PDK_ROOT/$PDK/libs.tech/xschem/xschemrc \"%s\" &)", file_path, pdk_value, file_path);
        g_print("Membuka skematik: %s\n", command);
    } else if (g_str_has_suffix(file_path, ".mag")) {
        snprintf(command, sizeof(command), "(cd \"$(dirname \"%s\")\" && export PDK=%s && magic -d XR -rcfile $PDK_ROOT/$PDK/libs.tech/magic/${PDK}.magicrc \"%s\" &)", file_path, pdk_value, file_path);
        g_print("Membuka layout: %s\n", command);
    } else {
        g_print("Tipe file tidak didukung: %s\n", file_path);
        return;
    }
    system(command);
}

// Callback untuk menghapus file setelah konfirmasi
static void on_delete_file_confirmed(GtkWidget *dialog, int response, gpointer data) {
    char *file_path = g_object_get_data(G_OBJECT(dialog), "file_path");
    ProjectManagerData *pm_data = g_object_get_data(G_OBJECT(dialog), "pm_data");

    if (response == GTK_RESPONSE_YES) {
        if (remove(file_path) == 0) {
            g_print("File berhasil dihapus: %s\n", file_path);
            on_refresh_clicked(NULL, pm_data); // Refresh the view
        } else {
            perror("Gagal menghapus file");
            GtkWidget *error_dialog = gtk_message_dialog_new(GTK_WINDOW(pm_data->window),
                                                             GTK_DIALOG_DESTROY_WITH_PARENT | GTK_DIALOG_MODAL,
                                                             GTK_MESSAGE_ERROR, GTK_BUTTONS_OK,
                                                             "Gagal menghapus file:\n%s", file_path);
            g_signal_connect_swapped(error_dialog, "response", G_CALLBACK(gtk_window_destroy), error_dialog);
            gtk_window_present(GTK_WINDOW(error_dialog));
        }
    }
    g_free(file_path);
    gtk_window_destroy(GTK_WINDOW(dialog));
}


// Callback saat tombol delete di navigator diklik
static void on_delete_file_clicked(GtkWidget *widget, gpointer data) {
    const char *file_path = (const char *)data;
    GtkWidget *toplevel = gtk_widget_get_ancestor(widget, GTK_TYPE_WINDOW);
    ProjectManagerData *pm_data = g_object_get_data(G_OBJECT(toplevel), "pm-data");

    GtkWidget *dialog = gtk_message_dialog_new(GTK_WINDOW(toplevel),
                                               GTK_DIALOG_DESTROY_WITH_PARENT | GTK_DIALOG_MODAL,
                                               GTK_MESSAGE_QUESTION,
                                               GTK_BUTTONS_YES_NO,
                                               "Are you sure you want to permanently delete this file?\n\n%s",
                                               file_path);
    gtk_window_set_title(GTK_WINDOW(dialog), "Confirm Delete File");

    g_object_set_data(G_OBJECT(dialog), "file_path", g_strdup(file_path));
    g_object_set_data(G_OBJECT(dialog), "pm_data", pm_data);
    g_signal_connect(dialog, "response", G_CALLBACK(on_delete_file_confirmed), NULL);
    gtk_window_present(GTK_WINDOW(dialog));
}


// Generic function to populate a file list
static void populate_design_files_list(ProjectManagerData *pm_data, const char *file_suffix, GtkWidget *list_box) {
    const char *project_path = g_object_get_data(G_OBJECT(pm_data->window), "current-project-path");
    if (project_path == NULL) return;

    const char *sub_dir = (strcmp(file_suffix, ".sch") == 0) ? "xschem" : "magic";
    char *scan_path = g_build_filename(project_path, sub_dir, NULL);


    GtkWidget *child;
    while ((child = gtk_widget_get_first_child(list_box))) {
        gtk_box_remove(GTK_BOX(list_box), child);
    }
    
    DIR *dir;
    struct dirent *ent;
    if ((dir = opendir(scan_path)) != NULL) {
        while ((ent = readdir(dir)) != NULL) {
            if (g_str_has_suffix(ent->d_name, file_suffix)) {
                GtkWidget *row_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6);
                
                GtkWidget *open_button = gtk_button_new_with_label(ent->d_name);
                gtk_widget_set_hexpand(open_button, TRUE);
                gtk_widget_set_halign(open_button, GTK_ALIGN_START);
                
                GtkWidget *delete_button = gtk_button_new_from_icon_name("edit-delete-symbolic");
                gtk_widget_add_css_class(delete_button, "destructive-action");
                
                gtk_box_append(GTK_BOX(row_box), open_button);
                gtk_box_append(GTK_BOX(row_box), delete_button);

                char *full_file_path = g_build_filename(scan_path, ent->d_name, NULL);

                g_signal_connect(open_button, "clicked", G_CALLBACK(on_design_file_clicked), g_strdup(full_file_path));
                g_signal_connect_swapped(open_button, "destroy", G_CALLBACK(g_free), g_strdup(full_file_path));
                
                g_signal_connect(delete_button, "clicked", G_CALLBACK(on_delete_file_clicked), g_strdup(full_file_path));
                
                gtk_box_append(GTK_BOX(list_box), row_box);
                g_free(full_file_path);
            }
        }
        closedir(dir);
    } else {
        g_print("Direktori tidak ditemukan atau tidak bisa dibuka: %s\n", scan_path);
    }
    g_free(scan_path);
}


static void on_create_design_file_clicked(GtkWidget *widget, gpointer data) {
    ProjectManagerData *pm_data = (ProjectManagerData *)data;
    const char *project_path = g_object_get_data(G_OBJECT(pm_data->window), "current-project-path");
    
    const char* button_name = gtk_widget_get_name(widget);
    gboolean is_schematic = (strcmp(button_name, "create_schematic_button") == 0);

    GtkEntry *entry = is_schematic ? GTK_ENTRY(pm_data->schematic_name_entry) : GTK_ENTRY(pm_data->layout_name_entry);
    const char *filename = gtk_editable_get_text(GTK_EDITABLE(entry));
    
    const char *sub_dir = is_schematic ? "xschem" : "magic";
    const char *suffix = is_schematic ? ".sch" : ".mag";
    const char *program = is_schematic ? "xschem --rcfile $PDK_ROOT/$PDK/libs.tech/xschem/xschemrc" : "magic -d XR -rcfile $PDK_ROOT/$PDK/libs.tech/magic/${PDK}.magicrc";
    
    if (project_path == NULL) { return; }

    if (filename == NULL || *filename == '\0') {
        GtkWidget *message_dialog = gtk_message_dialog_new(GTK_WINDOW(pm_data->window),
                                                        GTK_DIALOG_DESTROY_WITH_PARENT | GTK_DIALOG_MODAL,
                                                        GTK_MESSAGE_ERROR, GTK_BUTTONS_OK,
                                                        "Filename cannot be empty!");
        g_signal_connect_swapped(message_dialog, "response", G_CALLBACK(gtk_window_destroy), message_dialog);
        gtk_window_present(GTK_WINDOW(message_dialog));
        return;
    }

    char file_path[4096];
    char *base_path = g_build_filename(project_path, sub_dir, NULL);
    if (g_str_has_suffix(filename, suffix)) {
        snprintf(file_path, sizeof(file_path), "%s/%s", base_path, filename);
    } else {
        snprintf(file_path, sizeof(file_path), "%s/%s%s", base_path, filename, suffix);
    }
    g_free(base_path);


    char command[8192];
    snprintf(command, sizeof(command), "(cd \"$(dirname \"%s\")\" && export PDK=%s && %s \"%s\" &)", file_path, pdk_value, program, file_path);
    
    g_print("Menjalankan perintah: %s\n", command);
    system(command);
    
    on_refresh_clicked(NULL, pm_data);
    gtk_editable_set_text(GTK_EDITABLE(entry), "");
}

// Helper function to set up project after path is validated
static void setup_project(ProjectManagerData *pm_data, const char *folder_path) {
    char *old_path = g_object_steal_data(G_OBJECT(pm_data->window), "current-project-path");
    g_free(old_path);
    g_object_set_data(G_OBJECT(pm_data->window), "current-project-path", g_strdup(folder_path));

    char label_text[2048];
    snprintf(label_text, sizeof(label_text), "<b>Active Project:</b>\n<small>%s</small>", folder_path);
    gtk_label_set_markup(GTK_LABEL(pm_data->current_project_label), label_text);

    on_refresh_clicked(NULL, pm_data);
    gtk_widget_set_visible(pm_data->project_content_grid, TRUE);
}

// Callback for the permission dialog response
static void on_permission_response(GtkWidget *dialog, int response, gpointer user_data) {
    char *folder_path = g_object_get_data(G_OBJECT(dialog), "folder_path");
    ProjectManagerData *pm_data = (ProjectManagerData *)user_data;
    
    if (response == GTK_RESPONSE_YES) {
        const char *user_name = g_get_user_name();
        char command[4096];
        snprintf(command, sizeof(command), "sudo chown -R %s:%s \"%s\"", user_name, user_name, folder_path);
        g_print("take ownership: %s\n", command);
        int result = system(command);
        if (result == 0) {
            setup_project(pm_data, folder_path);
        } else {
             GtkWidget *fail_dialog = gtk_message_dialog_new(GTK_WINDOW(pm_data->window),
                                                             GTK_DIALOG_DESTROY_WITH_PARENT | GTK_DIALOG_MODAL,
                                                             GTK_MESSAGE_ERROR,
                                                             GTK_BUTTONS_OK,
                                                             "Failed to take ownership of the directory.");
             g_signal_connect_swapped(fail_dialog, "response", G_CALLBACK(gtk_window_destroy), fail_dialog);
             gtk_window_present(GTK_WINDOW(fail_dialog));
        }
    }
    
    g_free(folder_path);
    gtk_window_destroy(GTK_WINDOW(dialog));
}

static void on_open_project_response(GtkFileChooserDialog *dialog, int response, gpointer user_data) {
    ProjectManagerData *pm_data = (ProjectManagerData *)user_data;
    if (response == GTK_RESPONSE_ACCEPT) {
        GFile *folder = gtk_file_chooser_get_file(GTK_FILE_CHOOSER(dialog));
        char *folder_path = g_file_get_path(folder);

        if (g_access(folder_path, W_OK) != 0) {
            GtkWidget *perm_dialog = gtk_message_dialog_new(GTK_WINDOW(pm_data->window),
                                                            GTK_DIALOG_DESTROY_WITH_PARENT | GTK_DIALOG_MODAL,
                                                            GTK_MESSAGE_QUESTION,
                                                            GTK_BUTTONS_YES_NO,
                                                            "You do not have write permission for this project directory:\n\n%s\n\nDo you want to take ownership of this directory? (Requires administrator privileges)",
                                                            folder_path);
            gtk_window_set_title(GTK_WINDOW(perm_dialog), "Directory Permission Issue");
            
            g_object_set_data(G_OBJECT(perm_dialog), "folder_path", g_strdup(folder_path));
            g_signal_connect(perm_dialog, "response", G_CALLBACK(on_permission_response), pm_data);
            gtk_window_present(GTK_WINDOW(perm_dialog));

        } else {
             setup_project(pm_data, folder_path);
        }
        
        g_free(folder_path);
        g_object_unref(folder);
    }
    gtk_window_destroy(GTK_WINDOW(dialog));
}

static void on_open_project_clicked(GtkWidget *widget, gpointer data) {
    ProjectManagerData *pm_data = (ProjectManagerData *)data;
    GtkWidget *dialog = gtk_file_chooser_dialog_new("Open Project Directory",
                                                    GTK_WINDOW(pm_data->window),
                                                    GTK_FILE_CHOOSER_ACTION_SELECT_FOLDER,
                                                    "_Cancel", GTK_RESPONSE_CANCEL,
                                                    "_Open", GTK_RESPONSE_ACCEPT,
                                                    NULL);
    g_signal_connect(dialog, "response", G_CALLBACK(on_open_project_response), pm_data);
    gtk_window_present(GTK_WINDOW(dialog));
}

static void on_lvs_run_clicked(GtkWidget *widget, gpointer data) {
    ProjectManagerData *pm_data = (ProjectManagerData *)data;
    const char *project_path = g_object_get_data(G_OBJECT(pm_data->window), "current-project-path");
    const char *schematic_name = gtk_editable_get_text(GTK_EDITABLE(pm_data->lvs_schematic_entry));
    const char *layout_name = gtk_editable_get_text(GTK_EDITABLE(pm_data->lvs_layout_entry));
    const char *active_pdk = g_object_get_data(G_OBJECT(pm_data->window), "active-pdk-value");

    if (active_pdk == NULL) {
        GtkWidget *message_dialog = gtk_message_dialog_new(GTK_WINDOW(pm_data->window),
                                GTK_DIALOG_DESTROY_WITH_PARENT | GTK_DIALOG_MODAL,
                                GTK_MESSAGE_ERROR, GTK_BUTTONS_OK,
                                "Please select a PDK first from the dropdown menu!");
        g_signal_connect_swapped(message_dialog, "response", G_CALLBACK(gtk_window_destroy), message_dialog);
        gtk_window_present(GTK_WINDOW(message_dialog));
        return;
    }

    if (schematic_name == NULL || *schematic_name == '\0' || layout_name == NULL || *layout_name == '\0') {
        GtkWidget *message_dialog = gtk_message_dialog_new(GTK_WINDOW(pm_data->window),
                                GTK_DIALOG_DESTROY_WITH_PARENT | GTK_DIALOG_MODAL,
                                GTK_MESSAGE_ERROR, GTK_BUTTONS_OK,
                                "Schematic and layout file names for LVS cannot be empty!");
        g_signal_connect_swapped(message_dialog, "response", G_CALLBACK(gtk_window_destroy), message_dialog);
        gtk_window_present(GTK_WINDOW(message_dialog));
        return;
    }

    char lvs_script_command[8192];
    snprintf(lvs_script_command, sizeof(lvs_script_command), 
             "bash -c 'export PDK_ROOT=/usr/local/share/pdk; export PDK=%s; %s -w \"%s/lvs/%s\" -s \"%s/xschem/%s\" -l \"%s/magic/%s\" \"%s\"'", 
             active_pdk, lvs_script_path, project_path, schematic_name, project_path, schematic_name, project_path, layout_name, layout_name);

    g_print("--- Menjalankan LVS --- \n");
    g_print("Perintah yang akan dieksekusi: %s\n", lvs_script_command);
    
    int result = system(lvs_script_command); 

    GtkWidget *message_dialog;
    if (result == 0) {
        message_dialog = gtk_message_dialog_new(GTK_WINDOW(pm_data->window),
                                                GTK_DIALOG_DESTROY_WITH_PARENT | GTK_DIALOG_MODAL,
                                                GTK_MESSAGE_INFO, GTK_BUTTONS_OK,
                                                "Skrip LVS berhasil dijalankan.");
    } else {
        message_dialog = gtk_message_dialog_new(GTK_WINDOW(pm_data->window),
                                                GTK_DIALOG_DESTROY_WITH_PARENT | GTK_DIALOG_MODAL,
                                                GTK_MESSAGE_ERROR, GTK_BUTTONS_OK,
                                                "Gagal menjalankan skrip LVS.");
    }
    gtk_window_set_title(GTK_WINDOW(message_dialog), "Hasil Eksekusi LVS");
    g_signal_connect_swapped(message_dialog, "response", G_CALLBACK(gtk_window_destroy), message_dialog);
    gtk_window_present(GTK_WINDOW(message_dialog));
}

static void on_drc_run_clicked(GtkWidget *widget, gpointer data) {
    ProjectManagerData *pm_data = (ProjectManagerData *)data;
    const char *project_path = g_object_get_data(G_OBJECT(pm_data->window), "current-project-path");
    const char *layout_name = gtk_editable_get_text(GTK_EDITABLE(pm_data->drc_layout_entry));
    const char *active_pdk = g_object_get_data(G_OBJECT(pm_data->window), "active-pdk-value");

    if (active_pdk == NULL) {
        GtkWidget *message_dialog = gtk_message_dialog_new(GTK_WINDOW(pm_data->window),
                                                        GTK_DIALOG_DESTROY_WITH_PARENT | GTK_DIALOG_MODAL,
                                                        GTK_MESSAGE_ERROR, GTK_BUTTONS_OK,
                                                        "Pilih PDK terlebih dahulu dari menu dropdown!");
        g_signal_connect_swapped(message_dialog, "response", G_CALLBACK(gtk_window_destroy), message_dialog);
        gtk_window_present(GTK_WINDOW(message_dialog));
        return;
    }

    if (layout_name == NULL || *layout_name == '\0') {
        GtkWidget *message_dialog = gtk_message_dialog_new(GTK_WINDOW(pm_data->window),
                                                        GTK_DIALOG_DESTROY_WITH_PARENT | GTK_DIALOG_MODAL,
                                                        GTK_MESSAGE_ERROR, GTK_BUTTONS_OK,
                                                        "Nama file layout untuk DRC tidak boleh kosong!");
        g_signal_connect_swapped(message_dialog, "response", G_CALLBACK(gtk_window_destroy), message_dialog);
        gtk_window_present(GTK_WINDOW(message_dialog));
        return;
    }

    char drc_script_command[8192];
    snprintf(drc_script_command, sizeof(drc_script_command),
             "bash -c 'export PDK_ROOT=/usr/local/share/pdk; export PDK=%s; %s -w \"%s/drc/%s\" -l \"%s/magic/%s\" \"%s\"'",
             active_pdk, drc_script_path, project_path, layout_name, project_path, layout_name, layout_name);

    g_print("--- Menjalankan DRC --- \n");
    g_print("Perintah yang akan dieksekusi: %s\n", drc_script_command);

    int result = system(drc_script_command);

    GtkWidget *message_dialog;
    if (result == 0) {
        message_dialog = gtk_message_dialog_new(GTK_WINDOW(pm_data->window),
                                                GTK_DIALOG_DESTROY_WITH_PARENT | GTK_DIALOG_MODAL,
                                                GTK_MESSAGE_INFO, GTK_BUTTONS_OK,
                                                "Skrip DRC berhasil dijalankan.");
    } else {
        message_dialog = gtk_message_dialog_new(GTK_WINDOW(pm_data->window),
                                                GTK_DIALOG_DESTROY_WITH_PARENT | GTK_DIALOG_MODAL,
                                                GTK_MESSAGE_ERROR, GTK_BUTTONS_OK,
                                                "Gagal menjalankan skrip DRC.");
    }
    gtk_window_set_title(GTK_WINDOW(message_dialog), "Hasil Eksekusi DRC");
    g_signal_connect_swapped(message_dialog, "response", G_CALLBACK(gtk_window_destroy), message_dialog);
    gtk_window_present(GTK_WINDOW(message_dialog));
}


void on_refresh_clicked(GtkWidget *widget, gpointer data) {
    ProjectManagerData *pm_data = (ProjectManagerData *)data;
    const char *project_path = g_object_get_data(G_OBJECT(pm_data->window), "current-project-path");
    if (project_path == NULL) {
        g_print("Tidak ada proyek yang dibuka untuk diperbarui.\n");
        return;
    }
    
    g_print("Memperbarui daftar file...\n");
    populate_design_files_list(pm_data, ".sch", pm_data->schematic_list_box);
    populate_design_files_list(pm_data, ".mag", pm_data->layout_list_box);
    g_print("Daftar file telah diperbarui.\n");
}

static void on_pdk_changed(GtkDropDown *dropdown, GParamSpec *pspec, gpointer user_data) {
    ProjectManagerData *pm_data = (ProjectManagerData*)user_data;
    const char* selected_pdk = gtk_string_list_get_string(GTK_STRING_LIST(gtk_drop_down_get_model(dropdown)), gtk_drop_down_get_selected(dropdown));
    if (selected_pdk == NULL) return;
    
    if (strcmp(selected_pdk, "GF180MCU") == 0) {
        pdk_value = "gf180mcuD";
    } else if (strcmp(selected_pdk, "Sky130") == 0) {
        pdk_value = "sky130A";
    }
    
    g_object_set_data(G_OBJECT(pm_data->window), "active-pdk-value", (gpointer)pdk_value);
    g_print("PDK untuk sesi ini diatur ke: %s\n", pdk_value);

    GtkWidget *message_dialog = gtk_message_dialog_new(GTK_WINDOW(pm_data->window),
                                                GTK_DIALOG_DESTROY_WITH_PARENT | GTK_DIALOG_MODAL,
                                                GTK_MESSAGE_INFO, GTK_BUTTONS_OK,
                                                "PDK for this session has been set to %s.", selected_pdk);
    gtk_window_set_title(GTK_WINDOW(message_dialog), "Set PDK");
    g_signal_connect_swapped(message_dialog, "response", G_CALLBACK(gtk_window_destroy), message_dialog);
    gtk_window_present(GTK_WINDOW(message_dialog));
}

// Function to create a navigator section (frame with scrolled window and list box)
static GtkWidget* create_navigator_section(const char *title, GtkWidget **list_box_widget) {
    GtkWidget *frame = gtk_frame_new(title);
    GtkWidget *scrolled_window = gtk_scrolled_window_new();
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_window), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_widget_set_vexpand(scrolled_window, TRUE);
    gtk_frame_set_child(GTK_FRAME(frame), scrolled_window);
    
    *list_box_widget = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_widget_set_margin_top(*list_box_widget, 5);
    gtk_widget_set_margin_bottom(*list_box_widget, 5);
    gtk_widget_set_margin_start(*list_box_widget, 5);
    gtk_widget_set_margin_end(*list_box_widget, 5);
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scrolled_window), *list_box_widget);
    
    return frame;
}

// Function to create an action section (for creating new files)
static GtkWidget* create_action_section(const char *title, const char *placeholder, const char *button_name, GCallback callback, GtkWidget **entry_widget) {
    GtkWidget *frame = gtk_frame_new(title);
    GtkWidget *grid = gtk_grid_new();
    gtk_widget_set_margin_top(grid, 10);
    gtk_widget_set_margin_bottom(grid, 10);
    gtk_widget_set_margin_start(grid, 10);
    gtk_widget_set_margin_end(grid, 10);
    gtk_grid_set_column_spacing(GTK_GRID(grid), 10);
    gtk_frame_set_child(GTK_FRAME(frame), grid);

    *entry_widget = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(*entry_widget), placeholder);
    gtk_widget_set_hexpand(*entry_widget, TRUE);

    GtkWidget *button = gtk_button_new_with_label("Create & Open");
    gtk_widget_set_name(button, button_name);
    
    gtk_grid_attach(GTK_GRID(grid), *entry_widget, 0, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), button, 1, 0, 1, 1);
    
    g_signal_connect(button, "clicked", callback, NULL); 
    
    return frame;
}

static void show_project_manager_window(GtkWidget *widget, gpointer parent_window) {
    GtkWidget *window = gtk_window_new();
    gtk_window_set_title(GTK_WINDOW(window), "Project Manager");
    gtk_window_set_transient_for(GTK_WINDOW(window), GTK_WINDOW(parent_window));
    gtk_window_set_modal(GTK_WINDOW(window), TRUE);
    gtk_window_set_default_size(GTK_WINDOW(window), 800, -1);
    gtk_window_set_resizable(GTK_WINDOW(window), TRUE);


    GtkWidget *main_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 15);
    gtk_widget_set_margin_top(main_box, 20);
    gtk_widget_set_margin_bottom(main_box, 20);
    gtk_widget_set_margin_start(main_box, 20);
    gtk_widget_set_margin_end(main_box, 20);
    gtk_window_set_child(GTK_WINDOW(window), main_box);

    // --- Main Actions ---
    GtkWidget *main_actions_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6);
    GtkWidget *open_button = gtk_button_new_with_label("Open Project");
    GtkWidget *create_button = gtk_button_new_with_label("Create New Project");
    GtkWidget *refresh_button = gtk_button_new_from_icon_name("view-refresh-symbolic");
    gtk_box_append(GTK_BOX(main_actions_box), open_button);
    gtk_box_append(GTK_BOX(main_actions_box), create_button);
    gtk_box_append(GTK_BOX(main_actions_box), refresh_button);
    gtk_box_append(GTK_BOX(main_box), main_actions_box);
    
    gtk_box_append(GTK_BOX(main_box), gtk_separator_new(GTK_ORIENTATION_HORIZONTAL));

    // --- Active Project Info ---
    GtkWidget *active_project_frame = gtk_frame_new(NULL);
    GtkWidget *current_project_label = gtk_label_new("No project selected");
    gtk_label_set_xalign(GTK_LABEL(current_project_label), 0.0);
    gtk_widget_set_margin_top(current_project_label, 10);
    gtk_widget_set_margin_bottom(current_project_label, 10);
    gtk_frame_set_child(GTK_FRAME(active_project_frame), current_project_label);
    gtk_box_append(GTK_BOX(main_box), active_project_frame);

    // --- Project Content Grid (initially hidden) ---
    GtkWidget *project_content_grid = gtk_grid_new();
    gtk_grid_set_column_homogeneous(GTK_GRID(project_content_grid), TRUE);
    gtk_grid_set_column_spacing(GTK_GRID(project_content_grid), 15);
    gtk_grid_set_row_spacing(GTK_GRID(project_content_grid), 15);
    gtk_box_append(GTK_BOX(main_box), project_content_grid);
    gtk_widget_set_vexpand(project_content_grid, TRUE);
    
    // Create Data Struct
    ProjectManagerData *pm_data = g_new(ProjectManagerData, 1);
    pm_data->window = window;
    pm_data->current_project_label = current_project_label;
    pm_data->project_content_grid = project_content_grid;
    g_object_set_data(G_OBJECT(window), "pm-data", pm_data);


    // --- Project Settings Row ---
    GtkWidget *settings_frame = gtk_frame_new("Project Settings");
    gtk_grid_attach(GTK_GRID(project_content_grid), settings_frame, 0, 0, 2, 1); // Span 2 columns

    GtkWidget *settings_grid = gtk_grid_new();
    gtk_widget_set_margin_top(settings_grid, 10);
    gtk_widget_set_margin_bottom(settings_grid, 10);
    gtk_widget_set_margin_start(settings_grid, 10);
    gtk_widget_set_margin_end(settings_grid, 10);
    gtk_grid_set_column_spacing(GTK_GRID(settings_grid), 10);
    gtk_frame_set_child(GTK_FRAME(settings_frame), settings_grid);

    GtkWidget *pdk_label = gtk_label_new("Active PDK:");
    const char *pdk_items[] = {"GF180MCU", "Sky130", NULL};
    GtkStringList *pdk_string_list = gtk_string_list_new(pdk_items);
    pm_data->pdk_selection_dropdown = gtk_drop_down_new(G_LIST_MODEL(pdk_string_list), NULL);
    gtk_widget_set_hexpand(pm_data->pdk_selection_dropdown, TRUE);
    
    g_signal_connect(pm_data->pdk_selection_dropdown, "notify::selected", G_CALLBACK(on_pdk_changed), pm_data);


    gtk_grid_attach(GTK_GRID(settings_grid), pdk_label, 0, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(settings_grid), pm_data->pdk_selection_dropdown, 1, 0, 1, 1);


    // --- Schematic Column ---
    GtkWidget *schematic_col_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    //gtk_widget_set_vexpand(schematic_col_box, TRUE);
    gtk_widget_set_size_request(schematic_col_box, -1, 300); // 300 px tinggi minimum
    GtkWidget *schematic_navigator = create_navigator_section("Schematic (.sch)", &pm_data->schematic_list_box);
    GtkWidget *schematic_actions = create_action_section("New Schematic", "inverter.sch", "create_schematic_button", G_CALLBACK(on_create_design_file_clicked), &pm_data->schematic_name_entry);
    GtkWidget* create_schematic_button = gtk_grid_get_child_at(GTK_GRID(gtk_frame_get_child(GTK_FRAME(schematic_actions))), 1, 0);
    g_signal_handlers_disconnect_by_func(create_schematic_button, G_CALLBACK(on_create_design_file_clicked), NULL);
    g_signal_connect(create_schematic_button, "clicked", G_CALLBACK(on_create_design_file_clicked), pm_data);
    gtk_box_append(GTK_BOX(schematic_col_box), schematic_navigator);
    gtk_box_append(GTK_BOX(schematic_col_box), schematic_actions);
    gtk_grid_attach(GTK_GRID(project_content_grid), schematic_col_box, 0, 1, 1, 1);

    // --- Layout Column ---
    GtkWidget *layout_col_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    // Anda bisa mengubah ukuran tinggi navigator layout dengan mengatur vexpand dan/atau menambahkan widget pembatas ukuran.
    // Contoh: agar navigator layout tidak terlalu tinggi, Anda bisa membungkusnya dengan GtkScrolledWindow dan mengatur height request.
    //gtk_widget_set_vexpand(layout_col_box, TRUE); // Membuat kolom layout mengembang vertikal

    // Jika ingin membatasi tinggi minimum/maksimum:
    gtk_widget_set_size_request(layout_col_box, -1, 300); // 300 px tinggi minimum

    // Atau, batasi tinggi navigator layout saja (misal pada layout_navigator setelah dibuat):
    // gtk_widget_set_size_request(layout_navigator, -1, 250); // 250 px tinggi minimum
    GtkWidget *layout_navigator = create_navigator_section("Layout (.mag)", &pm_data->layout_list_box);
    GtkWidget *layout_actions = create_action_section("New Layout", "inverter.mag", "create_layout_button", G_CALLBACK(on_create_design_file_clicked), &pm_data->layout_name_entry);
    GtkWidget* create_layout_button = gtk_grid_get_child_at(GTK_GRID(gtk_frame_get_child(GTK_FRAME(layout_actions))), 1, 0);
    g_signal_handlers_disconnect_by_func(create_layout_button, G_CALLBACK(on_create_design_file_clicked), NULL);
    g_signal_connect(create_layout_button, "clicked", G_CALLBACK(on_create_design_file_clicked), pm_data);
    gtk_box_append(GTK_BOX(layout_col_box), layout_navigator);
    gtk_box_append(GTK_BOX(layout_col_box), layout_actions);
    gtk_grid_attach(GTK_GRID(project_content_grid), layout_col_box, 1, 1, 1, 1);
    
    // --- Verification Row ---
    GtkWidget *verification_frame = gtk_frame_new("Verification Tools (Under Construction)");
    gtk_grid_attach(GTK_GRID(project_content_grid), verification_frame, 0, 2, 2, 1);

    GtkWidget *verification_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 15);
    gtk_widget_set_margin_top(verification_box, 10);
    gtk_widget_set_margin_bottom(verification_box, 10);
    gtk_widget_set_margin_start(verification_box, 10);
    gtk_widget_set_margin_end(verification_box, 10);
    gtk_frame_set_child(GTK_FRAME(verification_frame), verification_box);

    // -- LVS --
    GtkWidget *lvs_grid = gtk_grid_new();
    gtk_grid_set_column_spacing(GTK_GRID(lvs_grid), 10);
    gtk_grid_set_row_spacing(GTK_GRID(lvs_grid), 5);
    GtkWidget *lvs_title_label = gtk_label_new("<b>LVS (Layout vs Schematic)</b>");
    gtk_label_set_use_markup(GTK_LABEL(lvs_title_label), TRUE);
    gtk_widget_set_halign(lvs_title_label, GTK_ALIGN_START);
    gtk_grid_attach(GTK_GRID(lvs_grid), lvs_title_label, 0, 0, 2, 1);
    
    GtkWidget *lvs_schematic_label = gtk_label_new("Schematic:");
    pm_data->lvs_schematic_entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(pm_data->lvs_schematic_entry), "file_name.sch");
    GtkWidget *lvs_layout_label = gtk_label_new("Layout:");
    pm_data->lvs_layout_entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(pm_data->lvs_layout_entry), "file_name.mag");
    GtkWidget *lvs_run_button = gtk_button_new_with_label("Jalankan LVS");
    gtk_widget_set_halign(lvs_run_button, GTK_ALIGN_END);

    gtk_grid_attach(GTK_GRID(lvs_grid), lvs_schematic_label, 0, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(lvs_grid), pm_data->lvs_schematic_entry, 1, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(lvs_grid), lvs_layout_label, 0, 2, 1, 1);
    gtk_grid_attach(GTK_GRID(lvs_grid), pm_data->lvs_layout_entry, 1, 2, 1, 1);
    gtk_grid_attach(GTK_GRID(lvs_grid), lvs_run_button, 1, 3, 1, 1);
    gtk_box_append(GTK_BOX(verification_box), lvs_grid);
    
    // -- DRC --
    gtk_box_append(GTK_BOX(verification_box), gtk_separator_new(GTK_ORIENTATION_HORIZONTAL));
    GtkWidget *drc_grid = gtk_grid_new();
    gtk_grid_set_column_spacing(GTK_GRID(drc_grid), 10);
    gtk_grid_set_row_spacing(GTK_GRID(drc_grid), 5);
    GtkWidget *drc_title_label = gtk_label_new("<b>DRC (Design Rule Check)</b>");
    gtk_label_set_use_markup(GTK_LABEL(drc_title_label), TRUE);
    gtk_widget_set_halign(drc_title_label, GTK_ALIGN_START);
    gtk_grid_attach(GTK_GRID(drc_grid), drc_title_label, 0, 0, 2, 1);

    GtkWidget *drc_layout_label = gtk_label_new("Layout:");
    pm_data->drc_layout_entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(pm_data->drc_layout_entry), "file_name.mag");
    GtkWidget *drc_run_button = gtk_button_new_with_label("Jalankan DRC");
    gtk_widget_set_halign(drc_run_button, GTK_ALIGN_END);

    gtk_grid_attach(GTK_GRID(drc_grid), drc_layout_label, 0, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(drc_grid), pm_data->drc_layout_entry, 1, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(drc_grid), drc_run_button, 1, 2, 1, 1);
    gtk_box_append(GTK_BOX(verification_box), drc_grid);

    gtk_widget_set_visible(project_content_grid, FALSE);

    // --- Connect Signals ---
    g_signal_connect(create_button, "clicked", G_CALLBACK(show_create_project_window), window);
    g_signal_connect(open_button, "clicked", G_CALLBACK(on_open_project_clicked), pm_data);
    g_signal_connect(refresh_button, "clicked", G_CALLBACK(on_refresh_clicked), pm_data);
    g_signal_connect(lvs_run_button, "clicked", G_CALLBACK(on_lvs_run_clicked), pm_data);
    g_signal_connect(drc_run_button, "clicked", G_CALLBACK(on_drc_run_clicked), pm_data);
    g_signal_connect_swapped(window, "destroy", G_CALLBACK(g_free), pm_data);

    gtk_window_present(GTK_WINDOW(window));
}


// --- Fungsi Utama untuk Membangun UI Jendela Utama---
static void activate(GtkApplication* app, gpointer user_data) {
    GtkWidget *window, *main_box, *main_label;
    GtkWidget *button_installer, *button_project_manager, *button_about;

    window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(window), "ITS Open-Source IC Suite");
    gtk_window_set_default_size(GTK_WINDOW(window), 350, -1);
    gtk_window_set_resizable(GTK_WINDOW(window), TRUE);

    main_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 15);
    gtk_widget_set_margin_top(main_box, 20);
    gtk_widget_set_margin_bottom(main_box, 20);
    gtk_widget_set_margin_start(main_box, 20);
    gtk_widget_set_margin_end(main_box, 20);
    gtk_window_set_child(GTK_WINDOW(window), main_box);
    
    main_label = gtk_label_new("Welcome!");
    gtk_label_set_markup(GTK_LABEL(main_label), "<big><b>Selamat Datang! Welcome!</b></big>");
    gtk_box_append(GTK_BOX(main_box), main_label);

    button_installer = gtk_button_new_with_label("Open Installer Manager");
    button_project_manager = gtk_button_new_with_label("Open Project Manager");
    button_about = gtk_button_new_with_label("About This Software");

    gtk_box_append(GTK_BOX(main_box), button_installer);
    gtk_box_append(GTK_BOX(main_box), button_project_manager);
    gtk_box_append(GTK_BOX(main_box), button_about);

    g_signal_connect(button_installer, "clicked", G_CALLBACK(show_installer_window), window);
    g_signal_connect(button_project_manager, "clicked", G_CALLBACK(show_project_manager_window), window);
    g_signal_connect(button_about, "clicked", G_CALLBACK(show_about_dialog), window);

    gtk_window_present(GTK_WINDOW(window));
}

// --- Fungsi Main ---
int main(int argc, char **argv) {
    GtkApplication *app;
    int status;

    app = gtk_application_new("com.its.opensuite.installer", G_APPLICATION_FLAGS_NONE);
    g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
    status = g_application_run(G_APPLICATION(app), argc, argv);
    g_object_unref(app);

    return status;
}