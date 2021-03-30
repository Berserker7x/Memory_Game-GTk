#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<gtk/gtk.h>
#include<glib.h>
#include <time.h>

//declaration des variables globales dont aura besoin dont differente fonctions

GtkBuilder *builder;// builder de l'interface
GtkWidget *bouttons[12];//tableau de bouttons
GtkWidget *image[12];//tableau des images
GtkWidget *imageclick[2];//deux click
GtkWidget *frstclick=NULL;//le premier click boutton
GtkWidget *scndclick=NULL;//deusieme click boutton
GtkWidget *score_label;//score label
GtkWidget *tries_label;//label des essais

void loginstartthegame();
void write_file();

int getimageindex(GtkButton *button), i = 0, cpt = 0, position = 0, losecounter = 3, score = 0, win, lose, counter = 0,last = 0;
char last_str[50], liste[100][100],*playerstr, *timee, score_str[50],tries_str[10];

gchar *buttons_GET[]={"boutton1","boutton2","boutton3","boutton4","boutton5","boutton6","boutton7","boutton8","boutton9","boutton10","boutton11","boutton12"};//insertion des boutton
gchar *images_link[]={"image1.jpg","image2.jpg","image3.jpg","image4.jpg","image5.jpg","image6.jpg","image1.jpg","image2.jpg","image3.jpg","image4.jpg","image5.jpg","image6.jpg"};//insertion des images
gchar *frstclick_img=NULL, *scndelick_img=NULL;//click des images

int n_1 = sizeof(buttons_GET) / sizeof(buttons_GET[0]);//taille du buttons_get
int n_2 = sizeof(images_link) / sizeof(images_link[0]);//taille du image_link

//fonction de permutation
void swap(gchar *A[], int c, int d) {

    int temp = A[c];
    A[c] = A[d];
    A[d] = temp;
}

//fonction de positionnement aleatoire des elements du tableau (algorithme fisher yates)
void shuffle(gchar *A[], int n){

    for (int a = n - 1; a >= 1; a--){
        int b = rand() % (a + 1);
        swap(A, a, b);
    }
}

//fonction d'affichage de la fenetre fin de partie
void winorlose(void){

    GtkWidget *end_game_window;//declaration de la fenetre
    GtkWidget *end;//button quitter
    GtkWidget *lose_label;//label echouer
    GtkWidget *win_label;//label gagner

    //insertion des elmnts de la fenetre
    end_game_window=GTK_WIDGET(gtk_builder_get_object(builder, "end_game_window"));
    end = GTK_WIDGET(gtk_builder_get_object(builder, "end"));
    lose_label = GTK_WIDGET(gtk_builder_get_object(builder, "lose_label"));
    win_label = GTK_WIDGET(gtk_builder_get_object(builder, "win_label"));

    //cas de victoire
    if(win==1){
     gtk_widget_show_all(end_game_window);
     gtk_widget_hide(lose_label);
    }

    //cas de défaite
    if(lose==1){
    gtk_widget_show_all(end_game_window);
    gtk_widget_hide(win_label);
    }

    g_signal_connect(end, "clicked", G_CALLBACK(write_file), NULL);//si on click sur quitter appel du write file pour memoriser le score
}

//fonction pour cacher les images s'ils sont pas les memes
gboolean nevrmindclick(gpointer data){

    //memorisation des indices par getimageindex
    int indice1 = getimageindex(GTK_BUTTON(imageclick[0]));
    int indice2 = getimageindex(GTK_BUTTON(imageclick[1]));

    //et on cache les deux images
    gtk_widget_hide(image[indice1]);
    gtk_widget_hide(image[indice2]);

  return G_SOURCE_REMOVE ;
}

//fonction d'enregistrement du score dans l'emplacement convenable
void score_submit(){

    sprintf(score_str,"%d",score);//transformation du score d'un entier en chaine de charactere
    strcpy(liste[position + 2],score_str);

}

//fonction pour le test de victoire  incrementation du cpt chaque reussite
int end_gamewin(void){
     cpt++;
         return cpt;
 }

//fonction pour memoriser l'indice de boutton
int getimageindex(GtkButton *button){

    int indice;

    for(int i = 0; i<12;i++){
        if(GTK_WIDGET(button)==bouttons[i]){
            indice = i;
        }
    }
    return indice;
}

//fonction qui se declenche apres un click sur une image
void eventafterclick(GtkButton *button, gpointer data){

    int indice = getimageindex(button);//stockage de l indice donné par la fct getimageinedex

    //insertion de label score et essai dans la fenetre
    score_label = GTK_WIDGET(gtk_builder_get_object(builder, "score_label"));
    tries_label = GTK_WIDGET(gtk_builder_get_object(builder, "tries_label"));

    // teste que tout est initialisé
    if(frstclick_img != NULL && scndelick_img != NULL){
        frstclick = NULL;
        scndclick = NULL;
        frstclick_img = NULL;
        scndelick_img = NULL;
    }

    //premier click et affichage de l'image de l'indice est correspendant
    if(frstclick_img == NULL){
        frstclick = GTK_WIDGET(button);
        frstclick_img = images_link[indice];
        gtk_widget_show(image[indice]);
        return;
    }

    //deuxiéme click  et affichage de l'image de l'indice est correspendant
    scndclick = GTK_WIDGET(button);
    scndelick_img = images_link[indice];
    gtk_widget_show(image[indice]);

    int yaysamecards = strcmp(frstclick_img,scndelick_img);//comparaison des deux images afficher

    if(yaysamecards == 0){ //oui ils sont les memes

        //initialisation des clicks a nouveau
        frstclick = NULL;
        scndclick = NULL;
        frstclick_img = NULL;
        scndelick_img = NULL;
        score += 100;// augemtation du score
        sprintf(score_str,"%d",score);//transformation du score d'un entier en chaine de charactere
        gtk_label_set_text(GTK_LABEL(score_label),score_str);//affichage de la label

        if(end_gamewin()==6){//test si les 6 images sont  bien choisie
            score_submit();
            //appel de winorlose en tant que win
            win=1;
            winorlose();
        }
        return;
    }

    // s'ils sont pas les memes
    else{

        //stockage des deux click et appel de nvrmindclick
        imageclick[0]=frstclick;
        imageclick[1]=scndclick;
        g_timeout_add(700,nevrmindclick,NULL);
        losecounter = losecounter - 1;//decrementation des essaies
        sprintf(tries_str,"%d",losecounter);//mise a jrs de label essaie
        gtk_label_set_text(GTK_LABEL(tries_label),tries_str);

        if( score >= 50){
            score = score - 50;//decrementation du score
            sprintf(score_str,"%d",score);//transformation du score d'un entier en chaine de charactere
            gtk_label_set_text(GTK_LABEL(score_label),score_str);
        }

        //fin des essaies et appel de winorlose en tant que lose
        if(losecounter==0){
            score_submit();
            lose=1;
            winorlose();
        }
    return;
    }

}

//fonction de delai avant disparition des images
gboolean mask_images(gpointer data){

    int i=0;
    for(i=0;i<12;i++){
       gtk_widget_hide(GTK_WIDGET(image[i]));
    }
    return G_SOURCE_REMOVE ;
}

//affichages d' interface de depart de la partie apres succes du login
void loginstartthegame(){

    GtkWidget *startthegame;//nom de fentre
    GtkWidget *player;//non du joueur

    //afficghhage des elements de la fenetres
    player = GTK_WIDGET(gtk_builder_get_object(builder, "player"));
    startthegame = GTK_WIDGET(gtk_builder_get_object(builder, "startthegame"));
    score_label = GTK_WIDGET(gtk_builder_get_object(builder, "score_label"));
    tries_label = GTK_WIDGET(gtk_builder_get_object(builder, "tries_label"));

    //inserer compteur des essaies a essaie label
    sprintf(tries_str,"%d",losecounter);//transformation du compteur des essais d'un entier en chaine de charactere
    sprintf(score_str,"%d",score);//de meme pour le score
    gtk_label_set_text(GTK_LABEL(tries_label),tries_str);//adaptation de label des essaies
    gtk_label_set_text(GTK_LABEL(score_label),score_str);//adaptation de label score
    gtk_label_set_text(player,playerstr);//adaptation du label joueur

    //insertion des elmntns dans la fenetre
    for(int i=0;i<12;i++){

        bouttons[i]=GTK_WIDGET(gtk_builder_get_object(builder,buttons_GET[i]));

        image[i]=GTK_WIDGET(gtk_image_new_from_file(images_link[i]));

        gtk_button_set_image(GTK_BUTTON(bouttons[i]),image[i]);//inserer l'image sur la boutton
    }

    g_timeout_add(4000,mask_images,NULL);//delais de 4s ppour memoriser les positions

    //detection des click
    for(int i=0;i<12;i++){
    g_signal_connect(bouttons[i], "clicked", G_CALLBACK(eventafterclick), NULL);
    }

    gtk_widget_show_all(startthegame);
}

//fonction de saisie des donnes dans un fichier a la fin de la partie
void write_file(GtkWidget *widget, gpointer data){

    //remplissage du fichier par la liste des donnes jusqu'au dernier joueur

    FILE *file_1,*file_2;
    file_1 = fopen("player_data.txt", "w");
    for(int j = 0; j < last; j++){

        fprintf(file_1,"%s\n",liste[j]);
    }

    //stockage du compteur de joueurs
    fclose(file_1);
    sprintf(last_str,"%d",last);
    file_2 = fopen("last.txt", "w");
    fprintf(file_2,"%s",last_str);
    fclose(file_2);

    gtk_main_quit();
}

//fonction d'acquisition des donner d'apres le fichier
void check_file(){

    //remplissage de la liste par les donnes du joueur
    FILE *file_1,*file_2;
    file_1 = fopen("player_data.txt", "r+");

    for(int j=0; j<100; j++){
        fscanf(file_1,"%s",liste[j]);
    }
    fclose(file_1);

    //acuqisition du compteur de joueurs memorisé
    file_2 = fopen("last.txt", "r+");
    fscanf(file_2,"%s",last_str);
    fclose(file_2);
    last = atoi(last_str);
}

//fonction de l'identification
void login(GtkWidget *widget, gpointer data){


    int ok = 0;//variable de test des erreurs

    GtkWidget *entr_name;//detection de nom
    GtkWidget *entr_pwd;//detection de mot de passe
    GtkWidget *error_name_1;//Nom d'utilisateur introuvable
    GtkWidget *error_name_2;//Nom d'utilsateur deja existant
    GtkWidget *error_name_3;//Veuillez saisir un nom d'utilisateur
    GtkWidget *error_pwd_1;//Mot de passe eroné
    GtkWidget *error_pwd_2;//Veuillez saisir un mot de passe

    //inssertion des elemnts de la fentre
    entr_name = GTK_WIDGET(gtk_builder_get_object(builder, "entr_name"));
    entr_pwd = GTK_WIDGET(gtk_builder_get_object(builder, "entr_pwd"));
    error_name_1 = GTK_WIDGET(gtk_builder_get_object(builder, "error_name_1"));
    error_name_2 = GTK_WIDGET(gtk_builder_get_object(builder, "error_name_2"));
    error_name_3 = GTK_WIDGET(gtk_builder_get_object(builder, "error_name_3"));
    error_pwd_1 = GTK_WIDGET(gtk_builder_get_object(builder, "error_pwd_1"));
    error_pwd_2 = GTK_WIDGET(gtk_builder_get_object(builder, "error_pwd_2"));

    //test du nom d'utilisateur et mot de passe
    for(int j = 0; j<98; j = j + 3){
        if(strcmp("",gtk_entry_get_text(GTK_ENTRY(entr_name))) == 0){
            //Veuillez saisir un nom d'utilisateur
            ok = 1;
            gtk_widget_show(error_name_3);
            gtk_widget_hide(error_name_1);
            gtk_widget_hide(error_name_2);
            gtk_widget_hide(error_pwd_1);
            gtk_widget_hide(error_pwd_2);
        }
        else{
            if(strcmp("",gtk_entry_get_text(GTK_ENTRY(entr_pwd))) == 0){
                //Veuillez saisir un mot de passe
                ok = 1;
                gtk_widget_hide(error_name_3);
                gtk_widget_hide(error_name_1);
                gtk_widget_hide(error_name_2);
                gtk_widget_hide(error_pwd_1);
                gtk_widget_show(error_pwd_2);
            }
            else{
                if ( strcmp(liste[j],gtk_entry_get_text(GTK_ENTRY(entr_name))) == 0){
                    ok = 1;
                    if ( strcmp(liste[j + 1],gtk_entry_get_text(GTK_ENTRY(entr_pwd))) == 0 ){
                        //succes du login
                        playerstr = liste[j];
                        position = j;
                        loginstartthegame();
                        gtk_widget_hide(error_name_1);
                        gtk_widget_hide(error_name_2);
                        gtk_widget_hide(error_name_3);
                        gtk_widget_hide(error_pwd_1);
                        gtk_widget_hide(error_pwd_2);
                    }
                    else{
                        //mot de passe eroné
                        gtk_widget_show(error_pwd_1);
                        gtk_widget_hide(error_pwd_2);
                        gtk_widget_hide(error_name_1);
                        gtk_widget_hide(error_name_2);
                        gtk_widget_hide(error_name_3);
                    }
                }
            }
        }
    }

    //affichage d'erreur au cas de faute de nom d'utilisateur
    if ( ok == 0 ){
        gtk_widget_show(error_name_1);
        gtk_widget_hide(error_name_2);
        gtk_widget_hide(error_name_3);
        gtk_widget_hide(error_pwd_1);
        gtk_widget_hide(error_pwd_2);
    }
}

//fontion  permettant l inscription de l utilisateur
void signup(GtkWidget *widget, gpointer data){

    //declaration des variables
    int error = 0;

    GtkWidget *entr_name;//detection de nom
    GtkWidget *entr_pwd;//detection de mot de passe
    GtkWidget *error_name_1;//Nom d'utilisateur introuvable
    GtkWidget *error_name_2;//Nom d'utilsateur deja existant
    GtkWidget *error_name_3;//Veuillez saisir un nom d'utilisateur
    GtkWidget *error_pwd_1;//Mot de passe eroné
    GtkWidget *error_pwd_2;//Veuillez saisir un mot de passe

    //appel des element du builder "glade"
    entr_name = GTK_WIDGET(gtk_builder_get_object(builder, "entr_name"));
    entr_pwd = GTK_WIDGET(gtk_builder_get_object(builder, "entr_pwd"));
    error_name_1 = GTK_WIDGET(gtk_builder_get_object(builder, "error_name_1"));
    error_name_2 = GTK_WIDGET(gtk_builder_get_object(builder, "error_name_2"));
    error_name_3 = GTK_WIDGET(gtk_builder_get_object(builder, "error_name_3"));
    error_pwd_1 = GTK_WIDGET(gtk_builder_get_object(builder, "error_pwd_1"));
    error_pwd_2 = GTK_WIDGET(gtk_builder_get_object(builder, "error_pwd_2"));

    //test de l'existance d'un nom d'utilisateur similaire
    for(int j = 0; j<98; j = j + 3){
        if(strcmp("",gtk_entry_get_text(GTK_ENTRY(entr_name))) == 0){
            //Veuillez saisir un nom d'utilisateur
            gtk_widget_show(error_name_3);
            gtk_widget_hide(error_name_1);
            gtk_widget_hide(error_name_2);
            gtk_widget_hide(error_pwd_1);
            gtk_widget_hide(error_pwd_2);
            error = 1;
        }
        else{
            if(strcmp("",gtk_entry_get_text(GTK_ENTRY(entr_pwd))) == 0){
                //Veuillez saisir un mot de passe
                gtk_widget_hide(error_name_3);
                gtk_widget_hide(error_name_1);
                gtk_widget_hide(error_name_2);
                gtk_widget_hide(error_pwd_1);
                gtk_widget_show(error_pwd_2);
                error = 1;
            }
            else{
                if(strcmp(liste[j],gtk_entry_get_text(GTK_ENTRY(entr_name))) == 0){
                    //Nom d'utilsateur deja existant
                    gtk_widget_show(error_name_2);
                    gtk_widget_hide(error_name_1);
                    gtk_widget_hide(error_name_3);
                    gtk_widget_hide(error_pwd_1);
                    gtk_widget_hide(error_pwd_2);
                    error = 1;
                }
            }
        }
    }

    //insertion des donnees au cas d'abscence d'erreur
    if (error == 0){
        strcpy(liste[last],gtk_entry_get_text(GTK_ENTRY(entr_name)));
        strcpy(liste[last+1],gtk_entry_get_text(GTK_ENTRY(entr_pwd)));
        strcpy(liste[last+2],"0");

        gtk_widget_hide(error_name_2);
        gtk_widget_hide(error_name_1);
        gtk_widget_hide(error_name_3);
        gtk_widget_hide(error_pwd_1);
        gtk_widget_hide(error_pwd_2);

        last = last + 3;//incrementation du compteur de joueurs
    }
}

//Fonction d'affichage du leaderboard
void leaderboard(GtkWidget *widget, gpointer data){

    GtkWidget *leaderboard_window;
    GtkWidget *leaderboard_fixed;
    GtkWidget *leaderboard_view;
    GtkTreeStore *leaderboard_treestore;
    GtkTreeView *leaderboard_tree;
    GtkTreeSelection *leaderboard_select;
    GtkTreeViewColumn *leaderboard_joueur;
    GtkCellRenderer *leaderboard_joueur_rd;
    GtkTreeViewColumn *leaderboard_score;
    GtkCellRenderer *leaderboard_score_rd;

    GtkTreeIter iterateur;

    leaderboard_window = GTK_WIDGET(gtk_builder_get_object(builder, "leaderboard_window"));
    leaderboard_fixed = GTK_WIDGET(gtk_builder_get_object(builder, "leaderboard_fixed"));
    leaderboard_view = GTK_WIDGET(gtk_builder_get_object(builder, "leaderboard_view"));
    leaderboard_treestore = GTK_TREE_STORE(gtk_builder_get_object(builder, "leaderboard_treestore"));
    leaderboard_tree = GTK_TREE_VIEW(gtk_builder_get_object(builder, "leaderboard_tree"));
    leaderboard_select = GTK_TREE_SELECTION(gtk_builder_get_object(builder, "leaderboard_select"));
    leaderboard_joueur = GTK_TREE_VIEW_COLUMN(gtk_builder_get_object(builder, "leaderboard_joueur"));
    leaderboard_joueur_rd = GTK_CELL_RENDERER(gtk_builder_get_object(builder, "leaderboard_joueur_rd"));
    leaderboard_score = GTK_TREE_VIEW_COLUMN(gtk_builder_get_object(builder, "leaderboard_score"));
    leaderboard_score_rd = GTK_CELL_RENDERER(gtk_builder_get_object(builder, "leaderboard_score_rd"));

    //syntaxe d'insertion des donnes dans les cases du tableau de leaderboard
    if ( counter == 0){
        gtk_tree_view_column_add_attribute(leaderboard_joueur,leaderboard_joueur_rd,"text",0);
        gtk_tree_view_column_add_attribute(leaderboard_score,leaderboard_score_rd,"text",1);

        for(int j = 0; j<99; j = j + 3){
            gtk_tree_store_append(leaderboard_treestore,&iterateur,NULL);
            gtk_tree_store_set(leaderboard_treestore,&iterateur,0,liste[j],-1);
            gtk_tree_store_set(leaderboard_treestore,&iterateur,1,liste[j+2],-1);
        }
        leaderboard_select = gtk_tree_view_get_selection(GTK_TREE_VIEW(leaderboard_tree));
        gtk_widget_show_all(leaderboard_window);
    }
    else{
        for(int j = 0; j<99; j = j + 3){
            gtk_tree_store_append(leaderboard_treestore,&iterateur,NULL);
            gtk_tree_store_set(leaderboard_treestore,&iterateur,0,liste[j],-1);
            gtk_tree_store_set(leaderboard_treestore,&iterateur,1,liste[j+2],-1);
        }
        leaderboard_select = gtk_tree_view_get_selection(GTK_TREE_VIEW(leaderboard_tree));
        gtk_widget_show_all(leaderboard_window);
    }
    counter += 1;

}

//fermuture de la fenetre de login
void close_login_window(GtkWidget *widget, gpointer data){

        GtkWidget *login_window;

        login_window = GTK_WIDGET(gtk_builder_get_object(builder, "login_window"));

        gtk_window_close(GTK_WINDOW(widget));

}

//fonction main du programme
int main(int argc, char **argv){

    gtk_init(&argc,&argv);

    // declaration des donnes de linterface
    GtkWidget *login_window; // la fenetre
    GtkWidget *login_button;// login button
    GtkWidget *signup_button;// signup button
    GtkWidget *leader_button;//leader button
    GtkWidget *exit_button;
    GtkWidget *error_name_1;// une label a afficher nom introuvable
    GtkWidget *error_name_2;//une troisieme en cas d erreur de mot de passe
    GtkWidget *error_name_3;
    GtkWidget *error_pwd_1;//une troisieme en cas d erreur de mot de passe
    GtkWidget *error_pwd_2;

    //affectation des elements de l'editeur glade dans le code
    builder = gtk_builder_new_from_file("Jeu.glade");
    login_window = GTK_WIDGET(gtk_builder_get_object(builder, "login_window"));
    login_button = GTK_WIDGET(gtk_builder_get_object(builder, "login_button"));
    signup_button = GTK_WIDGET(gtk_builder_get_object(builder, "signup_button"));
    leader_button = GTK_WIDGET(gtk_builder_get_object(builder, "leader_button"));
    exit_button = GTK_WIDGET(gtk_builder_get_object(builder, "exit_button"));
    error_name_1 = GTK_WIDGET(gtk_builder_get_object(builder, "error_name_1"));
    error_name_2 = GTK_WIDGET(gtk_builder_get_object(builder, "error_name_2"));
    error_name_3 = GTK_WIDGET(gtk_builder_get_object(builder, "error_name_3"));
    error_pwd_1 = GTK_WIDGET(gtk_builder_get_object(builder, "error_pwd_1"));
    error_pwd_2 = GTK_WIDGET(gtk_builder_get_object(builder, "error_pwd_2"));

    //gestion des donnes
    check_file();

    //reintialasation de la fonction rand()
    srand ( time(NULL) );

    //reorganization du tableau
    shuffle(images_link,n_2);
    shuffle(buttons_GET,n_1);

    // concection des boutton en evnmt de click avec les fentrres a afficher
    g_signal_connect(login_button, "clicked", G_CALLBACK(login), NULL);
    g_signal_connect(signup_button, "clicked", G_CALLBACK(signup), NULL);
    g_signal_connect(leader_button, "clicked", G_CALLBACK(leaderboard), NULL);
    g_signal_connect(exit_button, "clicked", G_CALLBACK(close_login_window), NULL);
    g_signal_connect(login_window, "delete_event", G_CALLBACK(write_file), NULL);

    // affichage total
    gtk_widget_show_all(login_window);

    //le masquage  des labels des erreurs
    gtk_widget_show_all(login_window);
    gtk_widget_hide(error_name_1);
    gtk_widget_hide(error_name_2);
    gtk_widget_hide(error_name_3);
    gtk_widget_hide(error_pwd_1);
    gtk_widget_hide(error_pwd_2);
    gtk_main();

}
