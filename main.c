#include <stdlib.h>
#include <stdio.h>
#include <windows.h>
#include <time.h>
#include <string.h>
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_ttf.h>
#include <SDL/fmod.h>

/** By Testelin Geoffrey **/

typedef int bool;
#define true 1
#define false 0

void color(int t,int f)  /** Fonction pour couleur console **/
{
    HANDLE H=GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(H,f*16+t);
}

void chevron(void)  /** Fonction pour couleur chevron **/
{
    color(10,0); /** Vert **/
    printf(">> ");
    color(15,0); /** Blanc **/
}

void baseColor(void) /** Fonction pour couleur de base **/
{
    color(15,0); /** Blanc **/
}

bool isCollide(SDL_Rect obj1, SDL_Rect obj2) /** Fonction qui détecte les collisions **/
{
    /** obj1 -> bird **/
    /** obj2 -> colonne **/

    /** Création des bords de l'image **/
    int leftA, leftB;
    int rightA, rightB;
    int topA, topB;
    int bottomA, bottomB;

    /** Obtention des coordonées du bird **/
    leftA = obj1.x;
    rightA = obj1.x + 40;
    topA = obj1.y;
    bottomA = obj1.y + 40;

    /** Obtention des coordonnées de la colonne **/
    leftB = obj2.x;
    rightB = obj2.x + 70;
    topB = obj2.y;
    bottomB = obj2.y + obj2.h;

    if (rightA <= leftB + 14) /** Pas de collision en zone transparente à gauche **/
    {
        return false;
    }

    if (leftA >= rightB - 14) /** Pas de collision en zone transparente à droite **/
    {
        return false;
    }

    if (bottomA <= topB)
    {
        return false;
    }

    if( topA >= bottomB )
    {
        return false;
    }

    return true; /** Collision **/
}

int main(int argc, char *argv[])
{
    int shutdown = 0; /** Lance/ferme la boucle du jeu **/
    int space_count = 0; /** Permet de mettre en  pause le jeu si  l'utilisateur n'a jamais appuiyé sur espace **/
    int tempsActuel = 0;
    int tempsPrecedent = 0;
    int gravite = 1; /**  Définit si la gravité est active ou non **/
    int newgame = 0; /** Permet de lancer une nouvelle partie en cas d'échec sans réinitialiser toutes les variables **/
    int tempsBirdActuel = 0;
    int tempsBirdPrecedent = 0;
    int birdJump = 0; /** Définit l'état du bird (en jump ou non) **/
    int nombreVie = 1;
    int tempsResuActuel = 0;
    int tempsResuPrecedent = 0;
    int need_resu = 0; /** Permet de savoir si une résurrection à besoin d'être effectuée ou non **/
    int dead_troll = 0; /** Variable pour troller les joueurs **/
    int dead_fond = 0; /** Permet de changer le fond en cas de bird mort **/
    int score_max = 0;
    int score_actuel = 0;
    int bonus_bomb = 2;
    int bonus_revival = 2;
    int bonus_shield = 2;
    int tempsColActuel = 0;
    int tempsColPrecedent = 0;
    int tempsBandeauActuel = 0;
    int tempsBandeauPrecedent = 0;
    int NbColonne1 = 50;
    int NbColonne2 = 50;
    int NbColonne3 = 50;
    int NbColonne4 = 50;
    int TypeColonne1[NbColonne1];
    int TypeColonne2[NbColonne2];
    int TypeColonne3[NbColonne3];
    int TypeColonne4[NbColonne4];
    int V = 0;
    int B = 0;
    int N = 0;
    int M = 0;
    int IdColonne1 = 1;
    int IdColonne2 = 1;
    int IdColonne3 = 1;
    int IdColonne4 = 1;
    int fall = 0; /** Permet de mettre l'oiseau en chute après une collision **/
    int mixeur = 1; /** Permet de créer nos tableaux pour modéliser les colonnes **/
    int hauteur_top_1 = 0, hauteur_top_2 = 0, hauteur_top_3 = 0, hauteur_top_4 = 0;
    int hauteur_bot_1 = 0, hauteur_bot_2 = 0, hauteur_bot_3 = 0, hauteur_bot_4 = 0;
    int hauteur_jump = 0; /** Permet de définir la hauteur max d'un jump **/
    int hauteur_fall = 0; /** Permet de définir la haute max d'un chute après jump **/
    int mega_jump = false;
    int autorisation_jump = false;
    int autorisation_fall = false;
    int use_bombe = false;
    int search_crash = true;
    int col_stop_1 = false;
    int col_stop_2 = false;
    int col_stop_3 = false;
    int col_stop_4 = false;

    baseColor();

    freopen("CON", "w", stdout); /** Activer lecture/ecriture console **/
    freopen("CON", "r", stdin);
    freopen("CON", "w", stderr);

    chevron(); printf("SURFACE_IN_PROGRESS\n");
    SDL_Surface *ecran = NULL;
    SDL_Surface *fond = NULL;
    SDL_Surface *fondClean = NULL;
    SDL_Surface *fondDead = NULL;
    SDL_Surface *bird = NULL;
    SDL_Surface *birdOff = NULL;
    SDL_Surface *birdDead = NULL;
    SDL_Surface *birdAngel = NULL;
    SDL_Surface *texteScoreMax = NULL;
    SDL_Surface *texteScoreActuel = NULL;
    SDL_Surface *texteBonusBomb = NULL;
    SDL_Surface *texteBonusRevival = NULL;
    SDL_Surface *texteBonusShield = NULL;
    chevron(); printf("SURFACE_CREATED\n");

    SDL_Event event; /** Variable d'événement **/

    TTF_Font *police = NULL; /** Création du pointeur police **/
    TTF_Font *police_small = NULL; /** Création du pointeur police_small **/
    TTF_Font *police_v_small = NULL; /** Création du pointeur police_v_small **/

    SDL_Color couleurNoire = {0, 0, 0};
    SDL_Color couleurRouge = {137, 11, 11};

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) == -1) /** Démarrage de la SDL. Si erreur : **/
    {
        fprintf(stderr, "Erreur d'initialisation de la SDL : %s\n", SDL_GetError()); /** Écriture de l'erreur **/
        exit(EXIT_FAILURE);
    }

    if(TTF_Init() == -1)
    {
        fprintf(stderr, "Erreur d'initialisation de TTF_Init : %s\n", TTF_GetError());
        exit(EXIT_FAILURE);
    }

    Uint32      colorkey;
    SDL_Surface    *icon;
    icon = SDL_LoadBMP("img/casque_romain.bmp"); /** Image de l'icone du jeu **/
    colorkey = SDL_MapRGB(icon->format, 255, 0, 0); /** Couleur rouge à retirée **/
    SDL_SetColorKey(icon, SDL_SRCCOLORKEY, colorkey); /** Application de la transparence **/
    SDL_WM_SetIcon(icon, NULL); /** Application icone **/

    ecran = SDL_SetVideoMode(800, 600, 32, SDL_HWSURFACE | SDL_DOUBLEBUF); /** Ouverture de la fenêtre **/

    SDL_WM_SetCaption("ROMANIZATION", NULL); /** Nom de la fenêtre **/

    chevron(); printf("RECTANGLE_IN_PROGRESS\n");
    SDL_Rect pos_fond;
    SDL_Rect pos_fond_clean;
    SDL_Rect pos_fond_dead;
    SDL_Rect pos_bird;
    SDL_Rect pos_bird_off;
    SDL_Rect pos_bird_dead;
    SDL_Rect pos_bird_angel;
    chevron(); printf("RECTANGLE_CREATED\n");

    chevron(); printf("CALCULATING_COORDINATES\n");
    pos_fond.x = 0, pos_fond.y = 0;
    pos_fond_clean.x = 0, pos_fond_clean.y = 0;
    pos_fond_dead.x = 0, pos_fond_dead.y = 0;
    pos_bird.x = 80, pos_bird.y = 225;
    pos_bird_off.x = 80, pos_bird_off.y = 225;
    pos_bird_dead.x = pos_bird.x, pos_bird_dead.y = pos_bird.y;
    pos_bird_angel.x = pos_bird.x, pos_bird_angel.y = pos_bird.y;
    chevron(); printf("COORDINATES_CALCULATED\n");

    chevron(); printf("LOAD_BMP_IN_PROGRESS\n");
    fond = SDL_LoadBMP("img/test_fond.bmp");
    fondClean = SDL_LoadBMP("img/test_fond_clean.bmp");
    fondDead = SDL_LoadBMP("img/test_fond_dead.bmp");
    bird = SDL_LoadBMP("img/test_bird.bmp");
    birdOff = SDL_LoadBMP("img/test_bird_off.bmp");
    birdDead = SDL_LoadBMP("img/test_bird_dead.bmp");
    birdAngel = SDL_LoadBMP("img/test_bird_angel.bmp");
    chevron(); printf("BMP_LOADED\n");

    chevron(); printf("CREATING_BANNER\n");
    SDL_Surface* bandeau;
    SDL_Rect TrueBandeau;
    SDL_Rect FakeBandeau;
    bandeau = SDL_LoadBMP("img/bandeau.bmp");
    TrueBandeau.x = 0;
    TrueBandeau.y = 530;
    chevron(); printf("BANNER_CREATED\n");

    chevron(); printf("CREATING_COLUMNS\n");
    SDL_Surface *col_100 = NULL, *col_100_bot = NULL;
    SDL_Surface *col_120 = NULL, *col_120_bot = NULL;
    SDL_Surface *col_140 = NULL, *col_140_bot = NULL;
    SDL_Surface *col_160 = NULL, *col_160_bot = NULL;
    SDL_Surface *col_180 = NULL, *col_180_bot = NULL;
    SDL_Surface *col_200 = NULL, *col_200_bot = NULL;
    SDL_Surface *col_220 = NULL, *col_220_bot = NULL;
    SDL_Surface *col_240 = NULL, *col_240_bot = NULL;
    SDL_Surface *col_260 = NULL, *col_260_bot = NULL;
    SDL_Surface *col_280 = NULL, *col_280_bot = NULL;

    SDL_Rect col_1_true, col_2_true, col_3_true, col_4_true;
    SDL_Rect col_1_fake, col_2_fake, col_3_fake, col_4_fake;
    SDL_Rect col_1_bot_true, col_2_bot_true, col_3_bot_true, col_4_bot_true;
    SDL_Rect col_1_bot_fake, col_2_bot_fake, col_3_bot_fake, col_4_bot_fake;

    col_100 = SDL_LoadBMP("img/100.bmp"); col_100_bot = SDL_LoadBMP("img/100_bot.bmp");
    col_120 = SDL_LoadBMP("img/120.bmp"); col_120_bot = SDL_LoadBMP("img/120_bot.bmp");
    col_140 = SDL_LoadBMP("img/140.bmp"); col_140_bot = SDL_LoadBMP("img/140_bot.bmp");
    col_160 = SDL_LoadBMP("img/160.bmp"); col_160_bot = SDL_LoadBMP("img/160_bot.bmp");
    col_180 = SDL_LoadBMP("img/180.bmp"); col_180_bot = SDL_LoadBMP("img/180_bot.bmp");
    col_200 = SDL_LoadBMP("img/200.bmp"); col_200_bot = SDL_LoadBMP("img/200_bot.bmp");
    col_220 = SDL_LoadBMP("img/220.bmp"); col_220_bot = SDL_LoadBMP("img/220_bot.bmp");
    col_240 = SDL_LoadBMP("img/240.bmp"); col_240_bot = SDL_LoadBMP("img/240_bot.bmp");
    col_260 = SDL_LoadBMP("img/260.bmp"); col_260_bot = SDL_LoadBMP("img/260_bot.bmp");
    col_280 = SDL_LoadBMP("img/280.bmp"); col_280_bot = SDL_LoadBMP("img/280_bot.bmp");

    SDL_SetColorKey(col_100, SDL_SRCCOLORKEY, SDL_MapRGB(col_100->format, 255, 0, 0));
    SDL_SetColorKey(col_120, SDL_SRCCOLORKEY, SDL_MapRGB(col_120->format, 255, 0, 0));
    SDL_SetColorKey(col_140, SDL_SRCCOLORKEY, SDL_MapRGB(col_140->format, 255, 0, 0));
    SDL_SetColorKey(col_160, SDL_SRCCOLORKEY, SDL_MapRGB(col_160->format, 255, 0, 0));
    SDL_SetColorKey(col_180, SDL_SRCCOLORKEY, SDL_MapRGB(col_180->format, 255, 0, 0));
    SDL_SetColorKey(col_200, SDL_SRCCOLORKEY, SDL_MapRGB(col_200->format, 255, 0, 0));
    SDL_SetColorKey(col_220, SDL_SRCCOLORKEY, SDL_MapRGB(col_220->format, 255, 0, 0));
    SDL_SetColorKey(col_240, SDL_SRCCOLORKEY, SDL_MapRGB(col_240->format, 255, 0, 0));
    SDL_SetColorKey(col_260, SDL_SRCCOLORKEY, SDL_MapRGB(col_260->format, 255, 0, 0));
    SDL_SetColorKey(col_280, SDL_SRCCOLORKEY, SDL_MapRGB(col_280->format, 255, 0, 0));
    SDL_SetColorKey(col_100_bot, SDL_SRCCOLORKEY, SDL_MapRGB(col_100_bot->format, 255, 0, 0));
    SDL_SetColorKey(col_120_bot, SDL_SRCCOLORKEY, SDL_MapRGB(col_120_bot->format, 255, 0, 0));
    SDL_SetColorKey(col_140_bot, SDL_SRCCOLORKEY, SDL_MapRGB(col_140_bot->format, 255, 0, 0));
    SDL_SetColorKey(col_160_bot, SDL_SRCCOLORKEY, SDL_MapRGB(col_160_bot->format, 255, 0, 0));
    SDL_SetColorKey(col_180_bot, SDL_SRCCOLORKEY, SDL_MapRGB(col_180_bot->format, 255, 0, 0));
    SDL_SetColorKey(col_200_bot, SDL_SRCCOLORKEY, SDL_MapRGB(col_200_bot->format, 255, 0, 0));
    SDL_SetColorKey(col_220_bot, SDL_SRCCOLORKEY, SDL_MapRGB(col_220_bot->format, 255, 0, 0));
    SDL_SetColorKey(col_240_bot, SDL_SRCCOLORKEY, SDL_MapRGB(col_240_bot->format, 255, 0, 0));
    SDL_SetColorKey(col_260_bot, SDL_SRCCOLORKEY, SDL_MapRGB(col_260_bot->format, 255, 0, 0));
    SDL_SetColorKey(col_280_bot, SDL_SRCCOLORKEY, SDL_MapRGB(col_280_bot->format, 255, 0, 0));

    SDL_SetColorKey(bird, SDL_SRCCOLORKEY, SDL_MapRGB(bird->format, 255, 0, 0));
    SDL_SetColorKey(birdOff, SDL_SRCCOLORKEY, SDL_MapRGB(birdOff->format, 255, 0, 0));
    SDL_SetColorKey(birdAngel, SDL_SRCCOLORKEY, SDL_MapRGB(birdAngel->format, 255, 0, 0));
    SDL_SetColorKey(birdDead, SDL_SRCCOLORKEY, SDL_MapRGB(birdDead->format, 255, 0, 0));

    col_1_true.x = 800, col_1_true.y = 0, col_1_bot_true.x = 800;
    col_2_true.x = 1043, col_2_true.y = 0, col_2_bot_true.x = 1043;
    col_3_true.x = 1286, col_3_true.y = 0, col_3_bot_true.x = 1286;
    col_4_true.x = 1530, col_4_true.y = 0, col_4_bot_true.x = 1530;
    chevron(); printf("COLUMNS_CREATED\n");

    srand(time(NULL)); /** initialisation de rand pour créer une position aléatoire **/

    char nbToCharMax[20] = ""; /** Tableau pour convertion du score max **/
    char nbToCharActuel[20] = ""; /** Tableau pour convertion du score actuel **/
    char nbToCharBomb[20] = ""; /** Tableau pour convertion du bonus 'Bomb' **/
    char nbToCharRevival[20] = ""; /** Tableau pour convertion du bonus 'Revival' **/
    char nbToCharShield[20] = ""; /** Tableau pour convertion du bonus 'Shield' **/

    sprintf(nbToCharMax, "%d", score_max);
    chevron(); printf("MAX_SCORE_CREATED\n");
    sprintf(nbToCharActuel, "%d", score_actuel);
    chevron(); printf("CURRENT_SCORE_CREATED\n");
    sprintf(nbToCharBomb, "%d", bonus_bomb);
    chevron(); color(11,0); printf("%d", bonus_bomb); baseColor(); printf("_BONUS_BOMB_ADDED\n");
    sprintf(nbToCharRevival, "%d", bonus_revival);
    chevron(); color(11,0); printf("%d", bonus_revival); baseColor(); printf("_BONUS_REVIVAL_ADDED\n");
    sprintf(nbToCharShield, "%d", bonus_shield);
    chevron(); color(11,0); printf("%d", bonus_shield); baseColor(); printf("_BONUS_SHIELD_ADDED\n");

    police = TTF_OpenFont("police/dalek.ttf", 17); /** Police dalek de taille 17 **/
    police_small = TTF_OpenFont("police/dalek.ttf", 16); /** Police dalek de taille 16 **/
    police_v_small = TTF_OpenFont("police/dalek.ttf", 14); /** Police dalek de taille 14 **/
    chevron(); printf("FONTS_LOADED\n");

    TTF_SetFontStyle(police, TTF_STYLE_BOLD); /** Police en gras **/
    TTF_SetFontStyle(police_small, TTF_STYLE_BOLD); /** Police_small en gras **/
    TTF_SetFontStyle(police_v_small, TTF_STYLE_BOLD); /** Police_v_small  en gras **/
    chevron(); printf("FONTS_STYLE_LOADED\n");

    SDL_Rect pos_texteScoreMax;
    SDL_Rect pos_texteScoreActuel;
    SDL_Rect pos_texteBonusBomb;
    SDL_Rect pos_texteBonusRevival;
    SDL_Rect pos_texteBonusShield;

    if (score_max >= 10) /** Si score_max contient au moins deux char **/
    {
        texteScoreMax = TTF_RenderText_Shaded(police_v_small, nbToCharMax, couleurNoire, couleurRouge); /** Texte pour afficher score max **/
        pos_texteScoreMax.x = 754, pos_texteScoreMax.y = 561; /** Coordonnées du texte score max **/
    }
    else
    {
        texteScoreMax = TTF_RenderText_Shaded(police, nbToCharMax, couleurNoire, couleurRouge); /** Texte pour afficher score max **/
        pos_texteScoreMax.x = 755, pos_texteScoreMax.y = 558; /** Coordonnées du texte score max **/
    }

    if (score_actuel >= 10) /** Si score_actuel contient au moins deux char **/
    {
        texteScoreActuel = TTF_RenderText_Shaded(police_v_small, nbToCharActuel, couleurNoire, couleurRouge); /** Texte pour afficher score actuel **/
        pos_texteScoreActuel.x = 683, pos_texteScoreActuel.y = 561; /** Coordonnées du texte score actuel **/
    }
    else
    {
        texteScoreActuel = TTF_RenderText_Shaded(police, nbToCharActuel, couleurNoire, couleurRouge); /** Texte pour afficher score actuel **/
        pos_texteScoreActuel.x = 684, pos_texteScoreActuel.y = 558; /** Coordonnées du texte score actuel **/
    }

    texteBonusBomb = TTF_RenderText_Shaded(police_small, nbToCharBomb, couleurNoire, couleurRouge); /** Texte pour afficher bonus 'Bomb' **/
    texteBonusRevival = TTF_RenderText_Shaded(police_small, nbToCharRevival, couleurNoire, couleurRouge); /** Texte pour afficher bonus 'Revival' **/
    texteBonusShield = TTF_RenderText_Shaded(police_small, nbToCharShield, couleurNoire, couleurRouge); /** Texte pour afficher bonus 'Shield' **/
    chevron(); printf("TEXT_LOADED\n");

    pos_texteBonusBomb.x = 52, pos_texteBonusBomb.y = 580; /** Coordonnées du texte bonus 'Bomb' **/
    pos_texteBonusRevival.x = 118, pos_texteBonusRevival.y = 580; /** Coordonnées du texte bonus 'Revival' **/
    pos_texteBonusShield.x = 194, pos_texteBonusShield.y = 580; /** Coordonnées du texte bonus 'Shield' **/

    chevron(); color(10,0); printf("SYSTEM_BOOTUP\n"); baseColor();
    chevron(); printf("GAME_STARTED\n");
    chevron(); color(12,0); printf("WAITING_USER\n"); baseColor();
    chevron(); printf("PRESS_SPACE_TO_BEGIN\n");

    /** LE JEU COMMENCE ICI **/

    while (shutdown == 0) /** TANT QUE le jeu est actif **/
    {
        while (SDL_PollEvent(&event)) /** On attend un événement qu'on récupère dans event **/
        {
            switch(event.type) /** On teste le type d'événement **/
            {
                case SDL_QUIT: /** SI c'est un événement QUITTER **/
                    chevron(); printf("EXIT_CLICK_DETECTED\n");
                    chevron(); color(10,0); printf("GAME_SHUTDOWN\n"); baseColor();
                    shutdown = 1; /** La partie se termine **/
                break;

                case SDL_KEYDOWN: /** SI on appuis sur une touche **/
                    switch (event.key.keysym.sym) /** On vérifie cette touche **/
                    {
                        case SDLK_ESCAPE: /** SI c'est la touche ECHAP **/
                            chevron(); printf("KEY_ESCAPE_DOWN\n");
                            chevron(); color(10,0); printf("GAME_SHUTDOWN\n"); baseColor();
                            shutdown = 1; /** La partie se termine **/
                        break;

                        case SDLK_SPACE: /** SI c'est la touche ESPACE **/
                            if (gravite == 1 && fall == 0) /** SI la gravite est sur ON et que le bird ne tombe pas **/
                            {
                                if (space_count == 0) /** SI l'utilisateur n'a pas encore appuiyé sur ESPACE **/
                                {
                                    chevron(); printf("INITIALIZATION\n");
                                    chevron(); printf("GRAVITY_ON\n");
                                }

                                hauteur_jump = pos_bird.y - 90; /** Définit la hauteur max du jump en fonction de la position du bird **/
                                hauteur_fall = pos_bird.y - 30; /** Définit la hauteur max du fall en fonction de la position du bird **/
                                autorisation_jump = false;
                                space_count = space_count + 1; /** Incrémentation du compteur **/
                                birdJump = 1; /** Le bird passe en mode jump **/
                            }
                            else /** SI gravite est sur OFF, impossible de faire voler le bird **/
                            {
                                chevron(); color(12,0); printf("KEY_SPACE_NOT_ALLOWED\n"); baseColor();
                            }
                        break;

                        case SDLK_e: /** SI c'est la touche E **/
                            chevron(); printf("KEY_e_DOWN\n");
                            use_bombe = true;
                        break;

                        case SDLK_r: /** SI c'est la touche R **/
                            chevron(); printf("KEY_r_DOWN\n");
                            if (newgame == 1) /** SI on peut créer une nouvelle partie **/
                            {
                                pos_bird.x = 80, pos_bird.y = 225; /** Coordonnées du bird initiales **/
                                SDL_BlitSurface(fond, NULL, ecran, &pos_fond); /** On blit le fond **/
                                chevron(); printf("LOAD_NEW_BACKGROUND\n");
                                SDL_BlitSurface(bird, NULL, ecran, &pos_bird);
                                chevron(); printf("LOAD_NEW_BIRD\n");
                                chevron(); printf("RESET_VARIABLES\n"); /** On réinitialise les variables **/
                                shutdown = 0;
                                space_count = 0;
                                tempsActuel = 0;
                                tempsPrecedent = 0;
                                gravite = 1;
                                tempsBirdActuel = 0;
                                tempsBirdPrecedent = 0;
                                birdJump = 0;
                                nombreVie = 1;
                                tempsResuActuel = 0;
                                tempsResuPrecedent = 0;
                                need_resu = 0;
                                dead_troll = 0;
                                dead_fond = 0;
                                newgame = 0;
                                bonus_bomb = 0;
                                bonus_revival = 0;
                                bonus_shield = 0;
                                tempsColActuel = 0;
                                tempsColPrecedent = 0;
                                tempsBandeauActuel = 0;
                                tempsBandeauPrecedent = 0;
                                V = 0;
                                B = 0;
                                N = 0;
                                M = 0;
                                fall = 0;
                                mixeur = 1;
                                search_crash = true;
                                col_stop_1 = false;
                                col_stop_2 = false;
                                col_stop_3 = false;
                                col_stop_4 = false;
                                chevron(); printf("CREATING_NEW_GAME\n");
                                chevron(); printf("NEW_GAME_CREATED\n");
                            }
                            else /** SI pas possible de recommencer **/
                            {
                                chevron(); color(12,0); printf("KEY_r_NOT_ALLOWED\n");
                                chevron(); color(12,0); printf("BIRD_MUST_DIE\n");  baseColor();
                                chevron(); color(11,0); printf("%d", nombreVie); baseColor(); printf("_LIFE_REMAINING\n");
                            }
                        break;

                        default:
                        break;
                    }
                break;

                case SDL_KEYUP: /** SI une touche se relève **/
                    switch (event.key.keysym.sym)
                    {
                        case SDLK_SPACE: /** SI on relâche la touche ESPACE **/
                            if (gravite == 1)
                            {
                                birdJump = 0; /** Fin de jump **/
                                mega_jump = false; /** Fin de jump **/
                                autorisation_fall = false; /** Fin du jump **/
                            }
                        break;

                        default:
                        break;
                    }
                break;

                default:
                break;
            }
        }

        if (mixeur == 1) /** Si 1, on créé nos tableaux **/
        {
            for (NbColonne1 = 0; NbColonne1 < 50; NbColonne1 = NbColonne1 + 1) /** Pour chaque colonne, on génère un "modèle" aléatoire **/
            {
                TypeColonne1[NbColonne1] = rand()%10; /** Modèle de colonne (9 modèles) **/
            }

            for (NbColonne2 = 0; NbColonne2 < 50; NbColonne2 = NbColonne2 + 1)
            {
                TypeColonne2[NbColonne2] = rand()%10;
            }

            for (NbColonne3 = 0; NbColonne3 < 50; NbColonne3 = NbColonne3 + 1)
            {
                TypeColonne3[NbColonne3] = rand()%10;
            }

            for (NbColonne4 = 0; NbColonne4 < 50; NbColonne4 = NbColonne4 + 1)
            {
                TypeColonne4[NbColonne4] = rand()%10;
            }
            mixeur = 0; /** On interdit de recréer nos tableaux **/
        }

        if (gravite == 1 && space_count > 0) /** SI le jeu a débuté **/
        {
            SDL_BlitSurface(fond, NULL, ecran, &pos_fond); /** Blit du fond **/
            SDL_BlitSurface(texteScoreMax, NULL, ecran, &pos_texteScoreMax); /** Blit du score max **/
            SDL_BlitSurface(texteScoreActuel, NULL, ecran, &pos_texteScoreActuel); /** Blit du score actuel **/
            SDL_BlitSurface(texteBonusBomb, NULL, ecran, &pos_texteBonusBomb); /** Blit du bonus 'Bomb' **/
            SDL_BlitSurface(texteBonusRevival, NULL, ecran, &pos_texteBonusRevival); /** Blit du bonus 'Revival' **/
            SDL_BlitSurface(texteBonusShield, NULL, ecran, &pos_texteBonusShield); /** Blit du bonus 'Shield' **/

            if ((col_1_true.x == 68 || col_1_bot_true.x == 68) || (col_2_true.x == 68 || col_2_bot_true.x == 68) || (col_3_true.x == 68 || col_3_bot_true.x == 68) || (col_4_true.x == 68 || col_4_bot_true.x == 68)) /** SI le bird dépasse la colonne **/
            {
                score_actuel = score_actuel + 1; /** On incrémente le score actuel **/
                sprintf(nbToCharActuel, "%d", score_actuel);
                SDL_FreeSurface(texteScoreActuel); /** On supprime l'ancien score **/
                if (score_actuel >= 10) /** Si score_actuel contient au moins deux char **/
                {
                    texteScoreActuel = TTF_RenderText_Shaded(police_v_small, nbToCharActuel, couleurNoire, couleurRouge); /** Texte pour afficher score actuel **/
                    pos_texteScoreActuel.x = 683, pos_texteScoreActuel.y = 561; /** Coordonnées du texte score actuel **/
                }
                else
                {
                    texteScoreActuel = TTF_RenderText_Shaded(police, nbToCharActuel, couleurNoire, couleurRouge); /** Texte pour afficher score actuel **/
                    pos_texteScoreActuel.x = 687, pos_texteScoreActuel.y = 558; /** Coordonnées du texte score actuel **/
                }

                SDL_BlitSurface(texteScoreActuel, NULL, ecran, &pos_texteScoreActuel); /** Blit du score actuel **/
            }

            tempsColActuel = SDL_GetTicks(); /** On récupère le temps écoulé **/
            if (tempsColActuel - tempsColPrecedent > 20) /** SI plus de 20 ms se sont écoulées **/
            {
                if (fall == 0) /** si pas de chute libre du bird on bouge les colonnes **/
                {
                    if (score_actuel <= 24)
                    {
                        col_1_true.x = col_1_true.x - 1;
                        col_1_bot_true.x = col_1_bot_true.x - 1;
                        col_2_true.x = col_2_true.x - 1;
                        col_2_bot_true.x = col_2_bot_true.x - 1;
                        col_3_true.x = col_3_true.x - 1;
                        col_3_bot_true.x = col_3_bot_true.x - 1;
                        col_4_true.x = col_4_true.x - 1;
                        col_4_bot_true.x = col_4_bot_true.x - 1;
                    }

                    if (score_actuel >= 25)
                    {
                        col_1_true.x = col_1_true.x - 2;
                        col_1_bot_true.x = col_1_bot_true.x - 2;
                        col_2_true.x = col_2_true.x - 2;
                        col_2_bot_true.x = col_2_bot_true.x - 2;
                        col_3_true.x = col_3_true.x - 2;
                        col_3_bot_true.x = col_3_bot_true.x - 2;
                        col_4_true.x = col_4_true.x - 2;
                        col_4_bot_true.x = col_4_bot_true.x - 2;
                    }
                }

                if (col_1_true.x <= -70 || col_1_bot_true.x <= -70) /** Si les col 1 sortent de l'écran **/
                {
                    col_1_true.x = 939; /** On repositionne les col 1 en dernières **/
                    col_1_bot_true.x = 939;
                    V = V + 1; /** Incrémentation du tableau des cas col 1 **/
                }

                if (col_2_true.x <= -70 || col_2_bot_true.x <= -70) /** Si les col 2 sortent de l'écran **/
                {
                    col_2_true.x = 939; /** On repositionne les col 2 en dernières **/
                    col_2_bot_true.x = 939;
                    B = B + 1; /** Incrémentation du tableau des cas col 2 **/
                }

                if (col_3_true.x <= -70 || col_3_bot_true.x <= -70) /** Si les col 3 sortent de l'écran **/
                {
                    col_3_true.x = 939; /** On repositionne les col 3 en dernières **/
                    col_3_bot_true.x = 939;
                    N = N + 1; /** Incrémentation du tableau des cas col 3 **/
                }

                if (col_4_true.x <= -70 || col_4_bot_true.x <= -70) /** Si les col 4 sortent de l'écran **/
                {
                    col_4_true.x = 939; /** On repositionne les col 4 en dernières **/
                    col_4_bot_true.x = 939;
                    M = M + 1; /** Incrémentation du tableau des cas col 4 **/
                }

                if (IdColonne1 == 1) /** Correspond aux valeurs des blit de la colonne 1 **/
                {
                    col_1_fake = col_1_true;
                    col_1_bot_fake = col_1_bot_true;
                    /** Les colonnes 1 prennent V comme référence de modèle **/

                    if (TypeColonne1[V] == 0)
                    {
                        col_1_bot_true.y = 250;
                        hauteur_top_1 = col_100 -> h;
                        hauteur_bot_1 = col_280_bot -> h;
                        SDL_BlitSurface(col_100, NULL, ecran, &col_1_fake);
                        SDL_BlitSurface(col_280_bot, NULL, ecran, &col_1_bot_fake);
                    }

                    if (TypeColonne1[V] == 1)
                    {
                        col_1_bot_true.y = 270;
                        hauteur_top_1 = col_120 -> h;
                        hauteur_bot_1 = col_260 -> h;
                        SDL_BlitSurface(col_120, NULL, ecran, &col_1_fake);
                        SDL_BlitSurface(col_260_bot, NULL, ecran, &col_1_bot_fake);
                    }

                    if (TypeColonne1[V] == 2)
                    {
                        col_1_bot_true.y = 290;
                        hauteur_top_1 = col_140 -> h;
                        hauteur_bot_1 = col_240 -> h;
                        SDL_BlitSurface(col_140, NULL, ecran, &col_1_fake);
                        SDL_BlitSurface(col_240_bot, NULL, ecran, &col_1_bot_fake);
                    }

                    if (TypeColonne1[V] == 3)
                    {
                        col_1_bot_true.y = 310;
                        hauteur_top_1 = col_160 -> h;
                        hauteur_bot_1 = col_220 -> h;
                        SDL_BlitSurface(col_160, NULL, ecran, &col_1_fake);
                        SDL_BlitSurface(col_220_bot, NULL, ecran, &col_1_bot_fake);
                    }

                    if (TypeColonne1[V] == 4)
                    {
                        col_1_bot_true.y = 330;
                        hauteur_top_1 = col_180 -> h;
                        hauteur_bot_1 = col_200 -> h;
                        SDL_BlitSurface(col_180, NULL, ecran, &col_1_fake);
                        SDL_BlitSurface(col_200_bot, NULL, ecran, &col_1_bot_fake);
                    }

                    if (TypeColonne1[V] == 5)
                    {
                        col_1_bot_true.y = 350;
                        hauteur_top_1 = col_200 -> h;
                        hauteur_bot_1 = col_180 -> h;
                        SDL_BlitSurface(col_200, NULL, ecran, &col_1_fake);
                        SDL_BlitSurface(col_180_bot, NULL, ecran, &col_1_bot_fake);
                    }

                    if (TypeColonne1[V] == 6)
                    {
                        col_1_bot_true.y = 370;
                        hauteur_top_1 = col_220 -> h;
                        hauteur_bot_1 = col_160 -> h;
                        SDL_BlitSurface(col_220, NULL, ecran, &col_1_fake);
                        SDL_BlitSurface(col_160_bot, NULL, ecran, &col_1_bot_fake);
                    }

                    if (TypeColonne1[V] == 7)
                    {
                        col_1_bot_true.y = 390;
                        hauteur_top_1 = col_240 -> h;
                        hauteur_bot_1 = col_140 -> h;
                        SDL_BlitSurface(col_240, NULL, ecran, &col_1_fake);
                        SDL_BlitSurface(col_140_bot, NULL, ecran, &col_1_bot_fake);
                    }

                    if (TypeColonne1[V] == 8)
                    {
                        col_1_bot_true.y = 410;
                        hauteur_top_1 = col_260 -> h;
                        hauteur_bot_1 = col_120 -> h;
                        SDL_BlitSurface(col_260, NULL, ecran, &col_1_fake);
                        SDL_BlitSurface(col_120_bot, NULL, ecran, &col_1_bot_fake);
                    }

                    if (TypeColonne1[V] == 9)
                    {
                        col_1_bot_true.y = 430;
                        hauteur_top_1 = col_280 -> h;
                        hauteur_bot_1 = col_100 -> h;
                        SDL_BlitSurface(col_280, NULL, ecran, &col_1_fake);
                        SDL_BlitSurface(col_100_bot, NULL, ecran, &col_1_bot_fake);
                    }
                }

                if (IdColonne2 == 1) /** Correspond aux valeurs des blit de la colonne 2 **/
                {
                    col_2_fake = col_2_true;
                    col_2_bot_fake = col_2_bot_true;
                    /** Les colonnes 2 prennent B comme référence de modèle **/

                    if (TypeColonne2[B] == 0)
                    {
                        col_2_bot_true.y = 250;
                        hauteur_top_2 = col_100 -> h;
                        hauteur_bot_2 = col_280_bot -> h;
                        SDL_BlitSurface(col_100, NULL, ecran, &col_2_fake);
                        SDL_BlitSurface(col_280_bot, NULL, ecran, &col_2_bot_fake);
                    }

                    if (TypeColonne2[B] == 1)
                    {
                        col_2_bot_true.y = 270;
                        hauteur_top_2 = col_120 -> h;
                        hauteur_bot_2 = col_260_bot -> h;
                        SDL_BlitSurface(col_120, NULL, ecran, &col_2_fake);
                        SDL_BlitSurface(col_260_bot, NULL, ecran, &col_2_bot_fake);
                    }

                    if (TypeColonne2[B] == 2)
                    {
                        col_2_bot_true.y = 290;
                        hauteur_top_2 = col_140 -> h;
                        hauteur_bot_2 = col_240_bot -> h;
                        SDL_BlitSurface(col_140, NULL, ecran, &col_2_fake);
                        SDL_BlitSurface(col_240_bot, NULL, ecran, &col_2_bot_fake);
                    }

                    if (TypeColonne2[B] == 3)
                    {
                        col_2_bot_true.y = 310;
                        hauteur_top_2 = col_160 -> h;
                        hauteur_bot_2 = col_220_bot -> h;
                        SDL_BlitSurface(col_160, NULL, ecran, &col_2_fake);
                        SDL_BlitSurface(col_220_bot, NULL, ecran, &col_2_bot_fake);
                    }

                    if (TypeColonne2[B] == 4)
                    {
                        col_2_bot_true.y = 330;
                        hauteur_top_2 = col_180 -> h;
                        hauteur_bot_2 = col_200_bot -> h;
                        SDL_BlitSurface(col_180, NULL, ecran, &col_2_fake);
                        SDL_BlitSurface(col_200_bot, NULL, ecran, &col_2_bot_fake);
                    }

                    if (TypeColonne2[B] == 5)
                    {
                        col_2_bot_true.y = 350;
                        hauteur_top_2 = col_200 -> h;
                        hauteur_bot_2 = col_180_bot -> h;
                        SDL_BlitSurface(col_200, NULL, ecran, &col_2_fake);
                        SDL_BlitSurface(col_180_bot, NULL, ecran, &col_2_bot_fake);
                    }

                    if (TypeColonne2[B] == 6)
                    {
                        col_2_bot_true.y = 370;
                        hauteur_top_2 = col_220 -> h;
                        hauteur_bot_2 = col_160_bot -> h;
                        SDL_BlitSurface(col_220, NULL, ecran, &col_2_fake);
                        SDL_BlitSurface(col_160_bot, NULL, ecran, &col_2_bot_fake);
                    }

                    if (TypeColonne2[B] == 7)
                    {
                        col_2_bot_true.y = 390;
                        hauteur_top_2 = col_240 -> h;
                        hauteur_bot_2 = col_140_bot -> h;
                        SDL_BlitSurface(col_240, NULL, ecran, &col_2_fake);
                        SDL_BlitSurface(col_140_bot, NULL, ecran, &col_2_bot_fake);
                    }

                    if (TypeColonne2[B] == 8)
                    {
                        col_2_bot_true.y = 410;
                        hauteur_top_2 = col_260 -> h;
                        hauteur_bot_2 = col_120_bot -> h;
                        SDL_BlitSurface(col_260, NULL, ecran, &col_2_fake);
                        SDL_BlitSurface(col_120_bot, NULL, ecran, &col_2_bot_fake);
                    }

                    if (TypeColonne2[B] == 9)
                    {
                        col_2_bot_true.y = 430;
                        hauteur_top_2 = col_280 -> h;
                        hauteur_bot_2 = col_100_bot -> h;
                        SDL_BlitSurface(col_280, NULL, ecran, &col_2_fake);
                        SDL_BlitSurface(col_100_bot, NULL, ecran, &col_2_bot_fake);
                    }
                }

                if (IdColonne3 == 1) /** Correspond aux valeurs des blit de la colonne 3 **/
                {
                    col_3_fake = col_3_true;
                    col_3_bot_fake = col_3_bot_true;
                    /** Les colonnes 3 prennent N comme référence de modèle **/

                    if (TypeColonne3[N] == 0)
                    {
                        col_3_bot_true.y = 250;
                        hauteur_top_3 = col_100 -> h;
                        hauteur_bot_3 = col_280_bot -> h;
                        SDL_BlitSurface(col_100, NULL, ecran, &col_3_fake);
                        SDL_BlitSurface(col_280_bot, NULL, ecran, &col_3_bot_fake);
                    }

                    if (TypeColonne3[N] == 1)
                    {
                        col_3_bot_true.y = 270;
                        hauteur_top_3 = col_120 -> h;
                        hauteur_bot_3 = col_260_bot -> h;
                        SDL_BlitSurface(col_120, NULL, ecran, &col_3_fake);
                        SDL_BlitSurface(col_260_bot, NULL, ecran, &col_3_bot_fake);
                    }

                    if (TypeColonne3[N] == 2)
                    {
                        col_3_bot_true.y = 290;
                        hauteur_top_3 = col_140 -> h;
                        hauteur_bot_3 = col_240_bot -> h;
                        SDL_BlitSurface(col_140, NULL, ecran, &col_3_fake);
                        SDL_BlitSurface(col_240_bot, NULL, ecran, &col_3_bot_fake);
                    }

                    if (TypeColonne3[N] == 3)
                    {
                        col_3_bot_true.y = 310;
                        hauteur_top_3 = col_160 -> h;
                        hauteur_bot_3 = col_220_bot -> h;
                        SDL_BlitSurface(col_160, NULL, ecran, &col_3_fake);
                        SDL_BlitSurface(col_220_bot, NULL, ecran, &col_3_bot_fake);
                    }

                    if (TypeColonne3[N] == 4)
                    {
                        col_3_bot_true.y = 330;
                        hauteur_top_3 = col_180 -> h;
                        hauteur_bot_3 = col_200_bot -> h;
                        SDL_BlitSurface(col_180, NULL, ecran, &col_3_fake);
                        SDL_BlitSurface(col_200_bot, NULL, ecran, &col_3_bot_fake);
                    }

                    if (TypeColonne3[N] == 5)
                    {
                        col_3_bot_true.y = 350;
                        hauteur_top_3 = col_200 -> h;
                        hauteur_bot_3 = col_180_bot -> h;
                        SDL_BlitSurface(col_200, NULL, ecran, &col_3_fake);
                        SDL_BlitSurface(col_180_bot, NULL, ecran, &col_3_bot_fake);
                    }

                    if (TypeColonne3[N] == 6)
                    {
                        col_3_bot_true.y = 370;
                        hauteur_top_3 = col_220 -> h;
                        hauteur_bot_3 = col_160_bot -> h;
                        SDL_BlitSurface(col_220, NULL, ecran, &col_3_fake);
                        SDL_BlitSurface(col_160_bot, NULL, ecran, &col_3_bot_fake);
                    }

                    if (TypeColonne3[N] == 7)
                    {
                        col_3_bot_true.y = 390;
                        hauteur_top_3 = col_240 -> h;
                        hauteur_bot_3 = col_140_bot -> h;
                        SDL_BlitSurface(col_240, NULL, ecran, &col_3_fake);
                        SDL_BlitSurface(col_140_bot, NULL, ecran, &col_3_bot_fake);
                    }

                    if (TypeColonne3[N] == 8)
                    {
                        col_3_bot_true.y = 410;
                        hauteur_top_3 = col_260 -> h;
                        hauteur_bot_3 = col_120_bot -> h;
                        SDL_BlitSurface(col_260, NULL, ecran, &col_3_fake);
                        SDL_BlitSurface(col_120_bot, NULL, ecran, &col_3_bot_fake);
                    }

                    if (TypeColonne3[N] == 9)
                    {
                        col_3_bot_true.y = 430;
                        hauteur_top_3 = col_280 -> h;
                        hauteur_bot_3 = col_100_bot -> h;
                        SDL_BlitSurface(col_280, NULL, ecran, &col_3_fake);
                        SDL_BlitSurface(col_100_bot, NULL, ecran, &col_3_bot_fake);
                    }
                }

                if (IdColonne4 == 1) /** Correspond aux valeurs des blit de la colonne 4 **/
                {
                    col_4_fake = col_4_true;
                    col_4_bot_fake = col_4_bot_true;
                    /** Les colonnes 4 prennent M comme référence de modèle **/

                    if (TypeColonne4[M] == 0)
                    {
                        col_4_bot_true.y = 250;
                        hauteur_top_4 = col_100 -> h;
                        hauteur_bot_4 = col_280_bot -> h;
                        SDL_BlitSurface(col_100, NULL, ecran, &col_4_fake);
                        SDL_BlitSurface(col_280_bot, NULL, ecran, &col_4_bot_fake);
                    }

                    if (TypeColonne4[M] == 1)
                    {
                        col_4_bot_true.y = 270;
                        hauteur_top_4 = col_120 -> h;
                        hauteur_bot_4 = col_260_bot -> h;
                        SDL_BlitSurface(col_120, NULL, ecran, &col_4_fake);
                        SDL_BlitSurface(col_260_bot, NULL, ecran, &col_4_bot_fake);
                    }

                    if (TypeColonne4[M] == 2)
                    {
                        col_4_bot_true.y = 290;
                        hauteur_top_4 = col_140 -> h;
                        hauteur_bot_4 = col_240_bot -> h;
                        SDL_BlitSurface(col_140, NULL, ecran, &col_4_fake);
                        SDL_BlitSurface(col_240_bot, NULL, ecran, &col_4_bot_fake);
                    }

                    if (TypeColonne4[M] == 3)
                    {
                        col_4_bot_true.y = 310;
                        hauteur_top_4 = col_160 -> h;
                        hauteur_bot_4 = col_220_bot -> h;
                        SDL_BlitSurface(col_160, NULL, ecran, &col_4_fake);
                        SDL_BlitSurface(col_220_bot, NULL, ecran, &col_4_bot_fake);
                    }

                    if (TypeColonne4[M] == 4)
                    {
                        col_4_bot_true.y = 330;
                        hauteur_top_4 = col_180 -> h;
                        hauteur_bot_4 = col_200_bot -> h;
                        SDL_BlitSurface(col_180, NULL, ecran, &col_4_fake);
                        SDL_BlitSurface(col_200_bot, NULL, ecran, &col_4_bot_fake);
                    }

                    if (TypeColonne4[M] == 5)
                    {
                        col_4_bot_true.y = 350;
                        hauteur_top_4 = col_200 -> h;
                        hauteur_bot_4 = col_180_bot -> h;
                        SDL_BlitSurface(col_200, NULL, ecran, &col_4_fake);
                        SDL_BlitSurface(col_180_bot, NULL, ecran, &col_4_bot_fake);
                    }

                    if (TypeColonne4[M] == 6)
                    {
                        col_4_bot_true.y = 370;
                        hauteur_top_4 = col_220 -> h;
                        hauteur_bot_4 = col_160_bot -> h;
                        SDL_BlitSurface(col_220, NULL, ecran, &col_4_fake);
                        SDL_BlitSurface(col_160_bot, NULL, ecran, &col_4_bot_fake);
                    }

                    if (TypeColonne4[M] == 7)
                    {
                        col_4_bot_true.y = 390;
                        hauteur_top_4 = col_240 -> h;
                        hauteur_bot_4 = col_140_bot -> h;
                        SDL_BlitSurface(col_240, NULL, ecran, &col_4_fake);
                        SDL_BlitSurface(col_140_bot, NULL, ecran, &col_4_bot_fake);
                    }

                    if (TypeColonne4[M] == 8)
                    {
                        col_4_bot_true.y = 410;
                        hauteur_top_4 = col_260 -> h;
                        hauteur_bot_4 = col_120_bot -> h;
                        SDL_BlitSurface(col_260, NULL, ecran, &col_4_fake);
                        SDL_BlitSurface(col_120_bot, NULL, ecran, &col_4_bot_fake);
                    }

                    if (TypeColonne4[M] == 9)
                    {
                        col_4_bot_true.y = 430;
                        hauteur_top_4 = col_280 -> h;
                        hauteur_bot_4 = col_100_bot -> h;
                        SDL_BlitSurface(col_280, NULL, ecran, &col_4_fake);
                        SDL_BlitSurface(col_100_bot, NULL, ecran, &col_4_bot_fake);
                    }
                }
                tempsColActuel = tempsColPrecedent; /** On met le temps actuel dans le temps precedent **/
                /** On donne les valeurs des hauteurs d'image à la hauteur des colonnes **/
                col_1_true.h = hauteur_top_1;
                col_1_bot_true.h = hauteur_bot_1;
                col_2_true.h = hauteur_top_2;
                col_2_bot_true.h = hauteur_bot_2;
                col_3_true.h = hauteur_top_3;
                col_3_bot_true.h = hauteur_bot_3;
                col_4_true.h = hauteur_top_4;
                col_4_bot_true.h = hauteur_bot_4;
            }
        }
        else if (nombreVie == 0) /** SI plus de vie **/
        {
            SDL_BlitSurface(fondDead, NULL, ecran, &pos_fond_dead); /** Blit du fond dead**/
            SDL_BlitSurface(birdDead, NULL, ecran, &pos_bird_dead); /** Blit du bird dead à la position du bird **/
            dead_fond = 1; /** Ok pour changer le fond **/
        }
        else /** SINON le jeu n'a pas débuté **/
        {
             SDL_BlitSurface(fondClean, NULL, ecran, &pos_fond_clean); /** Blit du fond clean **/
        }

        if (autorisation_jump == false && birdJump == 1 && pos_bird.y > hauteur_jump) /** SI le bird jump et qu'il n'a pas atteint la hauteur max **/
        {
            tempsBirdActuel = SDL_GetTicks(); /** On récupère le temps écoulé **/

            if (tempsBirdActuel - tempsBirdPrecedent > 20) /** SI plus de 20 ms se sont écoulées **/
            {
                pos_bird.y = pos_bird.y - 3; /** On déplace le bird vers le haut **/
                tempsBirdActuel = tempsBirdPrecedent; /** On met le temps actuel dans le temps precedent **/
            }
        }

        if (autorisation_jump == false && pos_bird.y <= hauteur_jump) /** SI le bird a atteint la hauteur max **/
        {
            birdJump = 0; /** On stop le jump; la gravite prend le relai **/
            mega_jump = true; /** Autorise le jump continu **/
        }

        if (pos_bird.y > hauteur_fall && mega_jump == true) /** SI le bird a atteint la position max de fall **/
        {
            autorisation_fall = true; /** Autorise le jump continu **/
        }

        if (autorisation_fall == true) /** Bird peut faire un jump continu **/
        {
            tempsBirdActuel = SDL_GetTicks(); /** On récupère le temps écoulé **/
            autorisation_jump = true;
            birdJump = 1; /** On remet le bird en mode jump **/

            if (tempsBirdActuel - tempsBirdPrecedent > 20) /** SI plus de 20 ms se sont écoulées **/
            {
                pos_bird.y = pos_bird.y - 4; /** On déplace le bird vers le haut **/
                tempsBirdActuel = tempsBirdPrecedent; /** On met le temps actuel dans le temps precedent **/
            }
        }

        if (use_bombe == true && gravite == 1 && space_count >= 1 && fall == 0) /** SI l'utilisateur à activer un bonus bomb et que la partie est en cours **/
        {
            if (bonus_bomb >= 1) /** SI il reste au moins 1 bombe et que la partie est en cours **/
            {
                bonus_bomb = bonus_bomb - 1; /** On perd 1 bonus bomb **/
                chevron(); printf("USER_LOST_"); color(11,0); printf("1"); baseColor(); printf("_BOMB\n");
                sprintf(nbToCharBomb, "%d", bonus_bomb);
                SDL_FreeSurface(texteBonusBomb); /** On supprime l'ancien nombre de bomb **/
                texteBonusBomb = TTF_RenderText_Shaded(police_small, nbToCharBomb, couleurNoire, couleurRouge); /** Texte pour afficher le bonus bomb **/
                pos_texteBonusBomb.x = 52, pos_texteBonusBomb.y = 580; /** Coordonnées du texte du bonus bomb **/
                SDL_BlitSurface(texteBonusBomb, NULL, ecran, &pos_texteBonusBomb); /** Blit du nombre de bomb **/
                chevron(); color(11,0); printf("%d", bonus_bomb); baseColor(); printf("_BOMB_REMAINING\n");
                /** On replace nos colonnes à la position de départ + 100 **/
                col_1_true.x = 900, col_1_true.y = 0, col_1_bot_true.x = 900;
                col_2_true.x = 1143, col_2_true.y = 0, col_2_bot_true.x = 1143;
                col_3_true.x = 1386, col_3_true.y = 0, col_3_bot_true.x = 1386;
                col_4_true.x = 1630, col_4_true.y = 0, col_4_bot_true.x = 1630;
                use_bombe = false; /** On termine nos effets du bonus bomb **/
            }
            else
            {
                chevron(); color(12,0); printf("NO_BOMB_REMAINING\n");
                use_bombe = false; /** On termine nos effets du bonus bomb **/
            }
        }

        if (search_crash == true) /** On recherche une collision **/
        {
            if (isCollide(pos_bird,col_1_true) || isCollide(pos_bird,col_2_true) || isCollide(pos_bird,col_3_true) || isCollide(pos_bird,col_4_true) ||
            isCollide(pos_bird,col_1_bot_true) || isCollide(pos_bird,col_2_bot_true) || isCollide(pos_bird,col_3_bot_true) || isCollide(pos_bird,col_4_bot_true))
            {
                chevron(); color(12,0); printf("COLLISION_DETECTED\n");
                birdJump = 0; /** Si collision alors on stop le jump du bird **/
                mega_jump = false; /** On stop la boucle de jump continu **/
                autorisation_fall = false; /** On stop la boucle de jump continu **/

                if (bonus_shield == 0) /** SI le nombre de bonus shield est à 0 alors la chute est de mise **/
                {
                    fall = 1; /** Collision, le bird tombe **/
                    chevron(); printf("BIRD_IS_FALLING\n");
                    search_crash = false; /** On arrête la recherche de collision **/
                }
                else /** SI il nous reste un shield **/
                {
                    if (isCollide(pos_bird,col_1_true))
                    {
                        col_1_true.x = col_1_true.x + 870; /** On dégage la colonne **/
                        col_stop_1 = true; /** On stop la collision de la colonne 1 **/
                        if (col_1_true.x == 800)
                        {
                            V = V + 1; /** Permet de changer le modèle de la colonne **/
                        }
                    }

                    if (isCollide(pos_bird,col_1_bot_true))
                    {
                        if (col_stop_1 == false)
                        {
                            col_1_bot_true.x = 870;

                            if (col_1_bot_true.x == 800)
                            {
                                V = V + 1;
                            }
                        }
                    }

                    if (isCollide(pos_bird,col_2_true))
                    {
                        col_2_true.x = col_2_true.x + 870;
                        col_stop_2 = true;
                        if (col_2_true.x == 800)
                        {
                            B = B + 1;
                        }
                    }

                    if (isCollide(pos_bird,col_2_bot_true))
                    {
                        if (col_stop_2 == false)
                        {
                            col_2_bot_true.x = col_2_bot_true.x + 870;
                            if (col_2_bot_true.x == 800)
                            {
                                B = B + 1;
                            }
                        }
                    }

                    if (isCollide(pos_bird,col_3_true))
                    {
                        col_3_true.x = col_3_true.x + 870;
                        col_stop_3 = true;
                        if (col_3_true.x == 800)
                        {
                            N = N + 1;
                        }
                    }

                    if (isCollide(pos_bird,col_3_bot_true))
                    {
                        if (col_stop_3 == false)
                        {
                            col_3_bot_true.x = col_3_bot_true.x + 870;
                            if (col_3_bot_true.x == 800)
                            {
                                N = N + 1;
                            }
                        }
                    }

                    if (isCollide(pos_bird,col_4_true))
                    {
                        col_4_true.x = col_4_true.x + 870;
                        col_stop_4 = true;
                        if (col_4_true.x == 800)
                        {
                            M = M + 1;
                        }
                    }

                    if (isCollide(pos_bird,col_4_true))
                    {
                        if (col_stop_4 == false)
                        {
                            col_4_bot_true.x = col_4_bot_true.x + 870;
                            if (col_4_bot_true.x == 800)
                            {
                                M = M + 1;
                            }
                        }
                    }
                    bonus_shield = bonus_shield - 1; /** On décrémente le nombre de bonus shield **/
                    chevron(); printf("PROTECTION_ACTIVATED\n");
                    chevron(); printf("USER_LOST_"); color(11,0); printf("%d", bonus_shield); baseColor(); printf("_SHIELD\n");
                    chevron(); color(11,0); printf("%d", bonus_shield); baseColor(); printf("_SHIELD_REMAINING\n");
                    sprintf(nbToCharShield, "%d", bonus_shield);
                    SDL_FreeSurface(texteBonusShield); /** On supprime l'ancien nombre de shield **/
                    texteBonusShield = TTF_RenderText_Shaded(police_small, nbToCharShield, couleurNoire, couleurRouge); /** Texte pour afficher le bonus shield **/
                    pos_texteBonusShield.x = 194, pos_texteBonusShield.y = 580; /** Coordonnées du texte du bonus shield **/
                    SDL_BlitSurface(texteBonusShield, NULL, ecran, &pos_texteBonusShield); /** Blit du nombre de shield **/
                }
            }
        }

        if (col_1_true.x == 500)
        {
            col_stop_1 = false;
        }

        if (col_2_true.x == 500)
        {
            col_stop_2 = false;
        }

        if (col_3_true.x == 500)
        {
            col_stop_3 = false;
        }

        if (col_4_true.x == 500)
        {
            col_stop_4 = false;
        }

        if (space_count >= 1) /** SI l'utilisateur n'a pas encore appuiyé sur ESPACE, pas de gravité **/
        {
            if (gravite == 1) /** SI la gravite est sur ON **/
            {
                if (birdJump == 0) /** SI le bird ne bouge pas **/
                {
                    tempsActuel = SDL_GetTicks(); /** On récupère le temps écoulé **/
                    if (tempsActuel - tempsPrecedent > 20) /** SI plus de 20 ms se sont écoulées **/
                    {
                        pos_bird.y = pos_bird.y + 2; /** On déplace le bird (gravite) **/
                        tempsActuel = tempsPrecedent; /** On met le temps actuel dans le temps precedent **/
                    }
                }
            }
        }

        tempsBandeauActuel = SDL_GetTicks(); /** On récupère le temps écoulé **/
        if (TrueBandeau.x <= -291) /** Si le schéma a était répété 1 fois (18 pixels) **/
        {
            TrueBandeau.x = 0; /** On le replace à l'état initial **/
        }

        if (tempsBandeauActuel - tempsBandeauPrecedent > 20) /** SI plus de 20 ms se sont écoulées **/
        {
            if (gravite == 1 && space_count > 0 && fall == 0) /** SI le jeu a débuté **/
            {
                TrueBandeau.x = TrueBandeau.x - 1; /** On déplace le bandeau **/
            }

            FakeBandeau = TrueBandeau; /** On copie les coordonnées du vrai bandeau **/
            SDL_BlitSurface(bandeau, NULL, ecran, &FakeBandeau); /** On déplace le faux bandeau (donc position x négative possible) **/
            tempsBandeauActuel = tempsBandeauPrecedent; /** On met le temps actuel dans le temps precedent **/
        }

        if (pos_bird.y >= 490 && newgame == 0) /** Si le bird touche le sol et que la boucle ne s'est pas encore lancée **/
        {
            fall = 0; /** Fin de la chute **/
            chevron(); printf("BIRD_HIT_GROUND\n");
            chevron(); printf("CURRENT_SCORE_IS_"); color(11,0); printf("%d\n", score_actuel);
            chevron(); printf("MAXIMUM_SCORE_IS_"); color(11,0); printf("%d\n", score_max);
            dead_troll = dead_troll + 1; /** On incrémente le troll **/
            chevron(); printf("BIRD_IS_DEAD_!!!\n");

            switch (dead_troll) /** Comment encourager un joueur **/
            {
                case 1:
                    color(13,0); printf(">>"); color(10,0); printf(" FAIL!\n");
                break;

                case 2:
                    color(13,0); printf(">>"); color(10,0); printf(" FAIL_AGAIN\n");
                break;

                case 3:
                    color(13,0); printf(">>"); color(10,0); printf(" TRY_HARDER\n");
                break;

                case 4:
                    color(13,0); printf(">>"); color(10,0); printf(" ITS_JUST_A_BIRD\n");
                break;

                case 5:
                    color(13,0); printf(">>"); color(10,0); printf(" A_FOOL_BIRD\n");
                break;

                case 6:
                    color(13,0); printf(">>"); color(10,0); printf(" FAIL!!\n");
                break;

                case 7:
                    color(13,0); printf(">>"); color(10,0); printf(" YOU_CAN_DO_IT\n");
                break;

                case 8:
                    color(13,0); printf(">>"); color(10,0); printf(" MAY_THE_FORCE_BE_WITH_YOU\n");
                break;

                case 9:
                    color(13,0); printf(">>"); color(10,0); printf(" GUINNESS_WORLD_RECORDS\n");
                break;

                case 10:
                    color(13,0); printf(">>"); color(10,0); printf(" YOU_SHALL_NOT_PASS\n");
                break;

                case 11:
                    color(13,0); printf(">>>>>>>>>><<<<<<<<<<\n");
                    color(13,0); printf(">>"); color(10,0); printf(" EXIT_OVER_HERE "); color(13,0); printf("<<\n");
                    color(13,0); printf(">>>>>>>>>><<<<<<<<<<\n");
                break;

                default:
                break;
            }

            if (bonus_revival == 0) /** Si plus aucun bonus revival **/
            {
                nombreVie = nombreVie - 1; /** On perd 1 vie **/
            }
            else
            {
                bonus_revival = bonus_revival - 1; /** On perd 1 bonus revival **/
                chevron(); printf("USER_LOST_"); color(11,0); printf("1"); baseColor(); printf("_REVIVAL\n");
                sprintf(nbToCharRevival, "%d", bonus_revival);
                SDL_FreeSurface(texteBonusRevival); /** On supprime l'ancien nombre de revival **/
                texteBonusRevival = TTF_RenderText_Shaded(police_small, nbToCharRevival, couleurNoire, couleurRouge); /** Texte pour afficher le bonus revival **/
                pos_texteBonusRevival.x = 118, pos_texteBonusRevival.y = 580; /** Coordonnées du texte du bonus revival **/
                SDL_BlitSurface(texteBonusRevival, NULL, ecran, &pos_texteBonusRevival); /** Blit du nombre de revival **/
                chevron(); color(11,0); printf("%d", bonus_revival); baseColor(); printf("_REVIVAL_REMAINING\n");
            }

            V = 0;
            B = 0;
            N = 0;
            M = 0;
            /** On remet les coordonnées initiales des colonnes **/
            col_1_true.x = 800, col_1_true.y = 0, col_1_bot_true.x = 800;
            col_2_true.x = 1043, col_2_true.y = 0, col_2_bot_true.x = 1043;
            col_3_true.x = 1286, col_3_true.y = 0, col_3_bot_true.x = 1286;
            col_4_true.x = 1530, col_4_true.y = 0, col_4_bot_true.x = 1530;

            if (nombreVie == 0) /** SI plus de vie **/
            {
                chevron(); printf("NO_LIFE_REMAINING\n");

                if (score_max < score_actuel) /** SI le score max est plus petit que l'actuel **/
                {
                    score_max = score_actuel; /** On enregistre un nouveau score max **/
                    chevron(); printf("NEW_BEST_SCORE!!\n");
                }
                score_actuel = 0; /** On efface le score actuel **/
                chevron(); printf("CURRENT_SCORE_RESET\n");
                sprintf(nbToCharActuel, "%d", score_actuel);
                SDL_FreeSurface(texteScoreActuel); /** On supprime l'ancien score **/
                texteScoreActuel = TTF_RenderText_Shaded(police, nbToCharActuel, couleurNoire, couleurRouge); /** Texte pour afficher score actuel **/
                pos_texteScoreActuel.x = 687, pos_texteScoreActuel.y = 558; /** Coordonnées du texte score actuel **/
                SDL_BlitSurface(texteScoreActuel, NULL, ecran, &pos_texteScoreActuel); /** Blit du score actuel **/
                sprintf(nbToCharMax, "%d", score_max);
                SDL_FreeSurface(texteScoreMax); /** On supprime l'ancien score **/

                if (score_max >= 10) /** Si score_max contient au moins deux char **/
                {
                    texteScoreMax = TTF_RenderText_Shaded(police_v_small, nbToCharMax, couleurNoire, couleurRouge); /** Texte pour afficher score max **/
                    pos_texteScoreMax.x = 754, pos_texteScoreMax.y = 561; /** Coordonnées du texte score max **/
                }
                else
                {
                    texteScoreMax = TTF_RenderText_Shaded(police, nbToCharMax, couleurNoire, couleurRouge); /** Texte pour afficher score max **/
                    pos_texteScoreMax.x = 755, pos_texteScoreMax.y = 558; /** Coordonnées du texte score max **/
                }
                SDL_BlitSurface(texteScoreMax, NULL, ecran, &pos_texteScoreMax); /** Blit du score max **/
                chevron(); printf("USER_LOST\n");
                chevron(); color(10,0); printf("GAME_OVER\n"); baseColor();
                chevron(); printf("GRAVITY_OFF\n");
                gravite = 0; /** La gravite s'arrête **/
                chevron(); printf("GAME_STOP\n");
                chevron(); printf("LOAD_NEW_BACKGROUND\n");
                chevron(); printf("LOAD_NEW_BIRD\n");
                pos_bird_dead = pos_bird; /** La position du bird dead prend la position du bird **/
                newgame = 1; /** Après un échec, on peut recommencer **/
                chevron(); printf("NEW_GAME_AUTHORIZED\n");
                chevron(); color(12,0); printf("PRESS_r_TO_RESET\n"); baseColor();
            }
            else /** SINON il peut revivre **/
            {
                chevron(); color(12,0); printf("GAME_IS_NOT_OVER\n"); baseColor();
                gravite = 0; /** La gravite est sur OFF **/
                pos_bird.y = 487; /** Corrige un bug qui fait perdre 2 vies d'un coup **/
                need_resu = 1; /** Autorise la résurrection **/
                chevron(); printf("RESURRECTION_AUTHORIZED\n");
                chevron(); printf("RESURRECTION_IN_PROGRESS\n");
                pos_bird_angel = pos_bird; /** On travaille avec le bird angel depuis les coordonnées du bird **/
                SDL_BlitSurface(birdAngel, NULL, ecran, &pos_bird_angel); /** Blit du bird angel à la position du bird **/
                search_crash = true; /** On autorise la recherche de collision **/
            }
        }

        if (need_resu == 1) /**  SI autoriser à faire une résurrection **/
        {
            SDL_BlitSurface(birdAngel, NULL, ecran, &pos_bird_angel); /** Blit du bird angel à la position du bird **/
            if (pos_bird_angel.y > 225) /** SI le bird angel n'est pas à la position initiale **/
            {
                SDL_Delay(5); /** Ralentie la résurrection **/
                tempsResuActuel = SDL_GetTicks(); /** On récupère le temps écoulé **/
                if (tempsResuActuel - tempsResuPrecedent > 20) /** SI plus de 20 ms se sont écoulées **/
                {
                    pos_bird_angel.y = pos_bird_angel.y - 1; /** On déplace le bird angel **/
                    tempsResuActuel = tempsResuPrecedent; /** On met le temps actuel dans le temps precedent **/
                }
            }

            if (pos_bird_angel.y == 225) /** SI le bird angel est à sa position initiale **/
            {
                chevron(); printf("RESURRECTION_COMPLETE\n");
                mixeur = 1; /** On réécrit nos tableaux **/
                pos_bird = pos_bird_angel; /** On donne la position du bird angel au bird **/

                if (bonus_shield == 0) /** Si plus de bonus bouclier **/
                {
                    pos_bird_off = pos_bird;
                    SDL_BlitSurface(birdOff, NULL, ecran, &pos_bird); /** Blit du bird sans bouclier **/
                }
                else
                {
                    SDL_BlitSurface(bird, NULL, ecran, &pos_bird); /** Blit du bird à la position du bird angel **/
                }
                space_count = 0; /** Variable réinitialiser, ESPACE pour continuer **/
                chevron(); color(12,0); printf("WAITING_USER\n"); baseColor();
                need_resu = 0; /** La variable reprend son état d'origine **/
                chevron(); printf("PRESS_SPACE_TO_BEGIN\n");
                gravite = 1; /** La gravite est sur ON **/
            }
        }

        if (dead_fond == 0 && need_resu == 0) /** Interdiction de blit le bird si dead_fond est afficher ou si la resu est en cours **/
        {
            if (bonus_shield == 0) /** Si plus de bonus bouclier **/
            {
                pos_bird_off = pos_bird;
                SDL_BlitSurface(birdOff, NULL, ecran, &pos_bird); /** Blit du bird sans bouclier **/
            }
            else
            {
                SDL_BlitSurface(bird, NULL, ecran, &pos_bird); /** Blit du bird à la position du bird angel **/
            }
        }

        SDL_Flip(ecran); /** On met à jour l'ecran **/
    }

    chevron(); printf("CALCULATING_SURFACES\n");
    chevron(); printf("SURFACES_CALCULATED\n");
    chevron(); printf("DELETE_SURFACES\n");
    SDL_FreeSurface(fond);
    SDL_FreeSurface(fondClean);
    SDL_FreeSurface(fondDead);
    SDL_FreeSurface(bird);
    SDL_FreeSurface(birdOff);
    SDL_FreeSurface(birdDead);
    SDL_FreeSurface(birdAngel);
    SDL_FreeSurface(bandeau);
    SDL_FreeSurface(col_100); SDL_FreeSurface(col_100_bot);
    SDL_FreeSurface(col_120); SDL_FreeSurface(col_120_bot);
    SDL_FreeSurface(col_140); SDL_FreeSurface(col_140_bot);
    SDL_FreeSurface(col_160); SDL_FreeSurface(col_160_bot);
    SDL_FreeSurface(col_180); SDL_FreeSurface(col_180_bot);
    SDL_FreeSurface(col_200); SDL_FreeSurface(col_200_bot);
    SDL_FreeSurface(col_220); SDL_FreeSurface(col_220_bot);
    SDL_FreeSurface(col_240); SDL_FreeSurface(col_240_bot);
    SDL_FreeSurface(col_260); SDL_FreeSurface(col_260_bot);
    SDL_FreeSurface(col_280); SDL_FreeSurface(col_280_bot);
    chevron(); printf("SURFACES_DELETED\n");

    TTF_CloseFont(police); /** Fermeture de la police **/
    TTF_CloseFont(police_small); /** Fermeture de la police_small **/
    TTF_CloseFont(police_v_small); /** Fermeture de la police_v_small **/
    TTF_Quit(); /** Arrêt de SDL_ttf **/

    SDL_FreeSurface(texteScoreMax);
    SDL_FreeSurface(texteScoreActuel);
    SDL_FreeSurface(texteBonusBomb);
    SDL_FreeSurface(texteBonusRevival);
    SDL_FreeSurface(texteBonusShield);
    chevron(); printf("BONUS_DELETED\n");
    chevron(); printf("SCORE_RESET\n");

    SDL_Quit(); /** Arrêt de la SDL **/
    chevron(); color(10,0); printf("ROMANIZATION_GOING_CLOSE\n"); baseColor();

    return EXIT_SUCCESS;
}
