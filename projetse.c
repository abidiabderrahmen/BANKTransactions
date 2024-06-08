#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#define max 100
//enregistrement du compte bancaire
typedef struct {
    int numcmp;
    float balance;
    char motpass[6];
} compte;

compte comptes[max];
int numcmp = 0;
//enregistrement des status de transaction
typedef struct {
    int numcmp;
    float montant;
    char type[10];
    char statut[10];
} transaction;

transaction transactions[max];
int numtrans = 0;
//cette fonction pour l'enregistrement des comptes dans test.txt
void enregfichier() {
    FILE* file = fopen("test.txt", "w");
    if (file == NULL) {
        printf("Impossible d'ouvrir le fichier pour l'enregistrement.\n");
        return;
    }

    for (int i = 0; i < numcmp; i++) {
        fprintf(file, "%d %.2f %s\n", comptes[i].numcmp, comptes[i].balance, comptes[i].motpass);
    }

    fclose(file);

    printf("Modifications enregistrées avec succès.\n");
}
//cette fonction pour l'enregistrement de transaction dans transaction.txt
void enregtrans() {
    FILE* file = fopen("transactions.txt", "w");
    if (file == NULL) {
        printf("Impossible d'ouvrir le fichier pour l'enregistrement des transactions.\n");
        return;
    }

    for (int i = 0; i < numtrans; i++) {
        fprintf(file, "%d %.2f %s %s\n", transactions[i].numcmp, transactions[i].montant, transactions[i].type, transactions[i].statut);
    }

    fclose(file);

    printf("Transactions enregistrées avec succès.\n");
}
//cette fonction pour ajouter les nouveaux comptes a un tab d'enregistrement puis le stockage permenant dans le fichier
void ajoutcmp() {
    if (numcmp >= max) {
        printf("Nombre maximal de comptes atteint.\n");
        return;
    }

    compte nouvcmp;

    printf("Numéro de compte : ");
    scanf("%d", &nouvcmp.numcmp);

    for (int i = 0; i < numcmp; i++) {
        if (comptes[i].numcmp == nouvcmp.numcmp) {
            printf("Un compte avec ce numéro existe déjà.\n");
            return;
        }
    }

    printf("Solde initial : ");
    scanf("%f", &nouvcmp.balance);

    printf("Mot de passe : ");
    scanf("%5s", nouvcmp.motpass);

    comptes[numcmp] = nouvcmp;
    numcmp++;

    enregfichier();

    printf("Compte ajouté avec succès.\n");
}
//la recherche d'un compte
int trouvcmp(int numcmp) {
    for (int i = 0; i < numcmp; i++) {
        if (comptes[i].numcmp == numcmp) {
            return i;
        }
    }
    return -1;
}
//cette fonction cherche le compte puis s'il existe on mmodifie le mot de passe et le change dans le fichier
void mot_de_passe(int numcmp) {
    int pos_cmp = trouvcmp(numcmp);
    if (pos_cmp == -1) {
        printf("Le compte n'existe pas.\n");
        return;
    }

    printf("Nouveau mot de passe : ");
    scanf("%5s", comptes[pos_cmp].motpass);

    enregfichier();

    printf("Mot de passe mis à jour avec succès.\n");
}

void depot(int numcmp, float montant) {
    pid_t pid;
    pid = fork();
    
    if (pid == 0) {
        sleep(2);
        printf("Dépôt effectué avec succès.\n");
        exit(0);
    } else if (pid > 0) {
        int pos_cmp = trouvcmp(numcmp);
        if (pos_cmp == -1) {
            printf("Le compte n'existe pas.\n");
            return;
        }

        comptes[pos_cmp].balance += montant;

        enregfichier();
    }
}

void retrait(int numcmp, float montant) {
    pid_t pid;
    pid = fork();
    
    if (pid == 0) {
        sleep(2);
        printf("Retrait effectué avec succès.\n");
        exit(0);
    } else if (pid > 0) {
        int pos_cmp = trouvcmp(numcmp);
        if (pos_cmp == -1) {
            printf("Le compte n'existe pas.\n");
            return;
        }
        
        if (comptes[pos_cmp].balance < montant) {
            printf("Solde insuffisants.\n");
            return;
        }

        comptes[pos_cmp].balance -= montant;

        enregfichier();
    } 
}

//ceci nous permet d'effectuer la suppression du compte dans le tableau puis enregistrer les modifications dans le fichier
void supprcompte(int numcmp) {
    int pos_cmp = trouvcmp(numcmp);
    if (pos_cmp == -1) {
        printf("Le compte n'existe pas.\n");
        return;
    }

    for (int i = pos_cmp; i < numcmp - 1 && i < max - 1; i++) {
        comptes[i] = comptes[i + 1];
    }

    numcmp--;

    enregfichier();

    printf("Compte supprimé avec succès.\n");
}

void transfer(int denumcmp, int anumcmp, float montant) {
    pid_t pid;
    pid = fork();
    
    if (pid == 0) {
        sleep(2);
        printf("Transfert effectué avec succès.\n");
        exit(0);
    } else if (pid > 0) {
        int depos_cmp = trouvcmp(denumcmp);
        if (depos_cmp == -1) {
            printf("Le compte source n'existe pas.\n");
            return;
        }

        int apos_cmp = trouvcmp(anumcmp);
        if (apos_cmp == -1) {
            printf("Le compte de destination n'existe pas.\n");
            return;
        }

        if (comptes[depos_cmp].balance < montant) {
            printf("Solde insuffisants dans le compte source.\n");
            return;
        }

        comptes[depos_cmp].balance -= montant;
        comptes[apos_cmp].balance += montant;

        enregfichier();
    }
}
//affichage du balance d'un client
void affichebalance(int numcmp, char* motpass) {
    int pos_cmp = trouvcmp(numcmp);
    if (pos_cmp == -1) {
        printf("Le compte n'existe pas.\n");
        return;
    }

    if (strcmp(comptes[pos_cmp].motpass, motpass) != 0) {
        printf("Mot de passe incorrect.\n");
        return;
    }

    printf("Solde du compte %d : %.2f\n", comptes[pos_cmp].numcmp, comptes[pos_cmp].balance);
}
//Afficher tous les clients de la banque et et ses informations
void affichetousclient() {
    printf("Liste des clients :\n");
    for (int i = 0; i < numcmp; i++) {
        printf("Numéro de compte : %d\n", comptes[i].numcmp);
        printf("Solde : %.2f\n", comptes[i].balance);
        printf("Mot de passe : %s\n", comptes[i].motpass);
        printf("-----------------------------\n");
    }
}
//affiche des transactions
void affichetrans() {
    printf("Liste des transactions :\n");
    for (int i = 0; i < numtrans; i++) {
        printf("Numéro de compte : %d\n", transactions[i].numcmp);
        printf("Montant : %.2f\n", transactions[i].montant);
        printf("Type : %s\n", transactions[i].type);
        printf("Statut : %s\n", transactions[i].statut);
        printf("-----------------------------\n");
    }
}


void menu() {
    int choix;
    int numcmp, anumcmp;
    float montant;
    char motpass[6];

    do {
        printf("\n-----------------------Menu de transaction de la Banque ----------------------------\n");
        printf("|                          1: Ajouter un compte                                     |\n");                                 
        printf("|                    2: Modifier votre mot de passe                                 |\n");                           
        printf("|                                3: Dépôt                                           |\n");
        printf("|                                4: retrait                                         |\n");
        printf("|                          5: Supprimer votre compte                                |\n");
        printf("|                          6: Transfert du solde                                    |\n");
        printf("|                         7: Afficher votre solde                                   |\n");
        printf("|      8: Afficher tous les clients(Seulement pour les personels de la banque)      |\n");
        printf("|         9: Afficher les transactions en attente ou en cours                       |\n");
        printf("|                               10: Quitter                                         |\n");
        printf("------------------------------------------------------------------------------------\n");
        printf("Choix : ");
        scanf("%d", &choix);

        switch (choix) {
            case 1:
                ajoutcmp();
                break;
            case 2:
                printf("Numéro de compte : ");
                scanf("%d", &numcmp);
                mot_de_passe(numcmp);
                break;
            case 3:
                printf("Numéro de compte : ");
                scanf("%d", &numcmp);
                printf("Montant du dépôt : ");
                scanf("%f", &montant);
                depot(numcmp, montant);
                break;
            case 4:
                printf("Numéro de compte : ");
                scanf("%d", &numcmp);
                printf("Montant du retrait : ");
                scanf("%f", &montant);
                retrait(numcmp, montant);
                break;
            case 5:
                printf("Numéro de compte : ");
                scanf("%d", &numcmp);
                supprcompte(numcmp);
                break;
            case 6:
                printf("Compte source : ");
                scanf("%d", &numcmp);
                printf("Compte de destination : ");
                scanf("%d", &anumcmp);
                printf("Montant du transfert : ");
                scanf("%f", &montant);
                transfer(numcmp, anumcmp, montant);
                break;
            case 7:
                printf("Numéro de compte : ");
                scanf("%d", &numcmp);
                printf("Mot de passe : ");
                scanf("%5s", motpass);
                affichebalance(numcmp, motpass);
                break;
            case 8:
                affichetousclient();
                break;
            case 9:
                affichetrans();
                break;
            case 10:
                printf("Programme terminé.\n");
                break;
            default:
                printf("Choix invalide. Veuillez réessayer.\n");
        }
    } while (choix != 10);
}

int main() {
    FILE *file;
    char buffer[100];
    int loadedComptes = 0;

    file = fopen("test.txt", "r+");

    if (file == NULL) {
        printf("Le fichier n'existe pas.\n");
    } else {
        while (fgets(buffer, sizeof(buffer), file) != NULL) {
            sscanf(buffer, "%d %f %s", &comptes[loadedComptes].numcmp, &comptes[loadedComptes].balance, comptes[loadedComptes].motpass);
            loadedComptes++;
        }
        numcmp = loadedComptes;
        fclose(file);
        printf("%d comptes chargés.\n", numcmp);
    }

    FILE *transFile;
    char tb[100];
    int lt = 0;

    transFile = fopen("transactions.txt", "r+");

    if (transFile == NULL) {
        printf("Le fichier des transactions n'existe pas.\n");
    } else {
        while (fgets(tb, sizeof(tb), transFile) != NULL) {
            sscanf(tb, "%d %f %s %s", &transactions[lt].numcmp, &transactions[lt].montant, transactions[lt].type, transactions[lt].statut);
            lt++;
        }
        numtrans = lt;
        fclose(transFile);
        printf("%d transactions chargées.\n", numtrans);
    }

    menu();

    enregfichier();
    enregtrans();

    return 0;
}
