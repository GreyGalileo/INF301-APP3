#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <stdbool.h>
#include "arbres.h"
#include "arbresphylo.h"
#include "listes.h"

void auxilliare_analyse_arbre (arbre racine, int* nb_esp, int* nb_carac);
// declaration de la fonction auxilliare

void analyse_arbre (arbre racine, int* nb_esp, int* nb_carac)
{
   *nb_esp = 0;
   *nb_carac = 0;
   auxilliare_analyse_arbre (racine, nb_esp, nb_carac);

}

void auxilliare_analyse_arbre (arbre racine, int* nb_esp, int* nb_carac) {
   if (racine == NULL){ return; }
   if (racine->gauche == NULL && racine->droit == NULL){
      (*nb_esp)++;
      return;
   }//une des branches n'est pas NULL;
   (*nb_carac)++;
   if (racine->gauche != NULL) {
      auxilliare_analyse_arbre(racine->gauche, nb_esp, nb_carac);
   }
   if (racine->droit != NULL) {
      auxilliare_analyse_arbre(racine->droit, nb_esp, nb_carac);
   }
}




/* ACTE II */
/* Recherche l'espece dans l'arbre. Modifie la liste passée en paramètre pour y mettre les
 * caractéristiques. Retourne 0 si l'espèce a été retrouvée, 1 sinon.
 */
int rechercher_espece (arbre racine, char *espece, liste_t* seq){
   if (racine == NULL){
      return 1;
   }
   if (strcmp(racine->valeur, espece) == 0){
      return 0;
   }
   int res;
   res =  rechercher_espece(racine->gauche, espece, seq);
   if (res == 0){
      return 0;//retourne car on a trouve l'espece dans l'arbre gauche
   }
  res =  rechercher_espece(racine->droit, espece, seq);
  if (res == 0){
    ajouter_tete(seq, racine->valeur);//est dans la branche droite donc a l'attribut de la cellule
    return 0;
   }
   return 1;//on n'a pas trouve l'espece dans les arbres gauche ou droits
}


//Acte I-b
void dot_format_conversion_auxilliary(arbre a, FILE *f){
   if (a == NULL){
      return;
   } if (a->droit != NULL){
      fprintf(f, "%s -> %s [label = \"oui\"]\n", a->valeur, a->droit->valeur);
      dot_format_conversion_auxilliary(a->droit, f);
   } if (a->gauche != NULL){
      fprintf(f, "%s -> %s [label = \"non\"]\n", a->valeur, a->gauche->valeur);
      dot_format_conversion_auxilliary(a->gauche, f);
   }
}

void dot_format_conversion(arbre a, FILE *f){
   fprintf(f, "digraph arbre {\n");
   dot_format_conversion_auxilliary(a, f);
   fprintf(f, "}");
}





/* Doit renvoyer 0 si l'espece a bien ete ajoutee, 1 sinon, et ecrire un 
 * message d'erreur.
 */
int ajouter_espece (arbre* a, char *espece, cellule_t* seq) {
   assert(a != NULL);

   if (*a == NULL && seq == NULL){
      //both the tree and list are empty, we insert the species
      noeud *new_branch = nouveau_noeud();
      new_branch->valeur = espece;
      (*a) = new_branch;
      return 0;
   } else if (*a == NULL && seq != NULL) {
      //tree is empty but there are still elements in the list
      noeud *new_branch = nouveau_noeud();
      new_branch->valeur = strdup(seq->val);
      (*a) = new_branch;
      return ajouter_espece(&((*a)->droit), espece, seq->suivant);
   } else if ((*a)->droit == NULL && (*a)->gauche == NULL && seq == NULL) {
      //seq is empty but tree is a species, we return an error
      printf("There is already a species in the tree with all the same characteristsics!");
      return 1;
   } else if ((*a)->droit == NULL && (*a)->gauche == NULL && seq != NULL) {
      //we reach a species and seq is nonempty, we add the characteristic and put the species to its left
      noeud *new_branch = nouveau_noeud();
      new_branch->valeur = strdup(seq->val);
      new_branch->gauche = *a;
      (*a) = new_branch;
      return ajouter_espece(&((*a)->droit), espece, seq->suivant);
   } else if ( seq != NULL && (strcmp((*a)->valeur, seq->val) == 0) ) {
      //we reach the charcterisitic in seq, we go recursively to the right
      return ajouter_espece(&((*a)->droit), espece, seq->suivant);
   } else if (seq != NULL  && (strcmp((*a)->valeur, seq->val) != 0) ) {
      //we reach the charcterisitic not in seq, we go recursively to the left
      return ajouter_espece(&((*a)->gauche), espece, seq);
   } else if ( seq == NULL && ((*a)->droit != NULL || (*a)->gauche != NULL) ) {
      //we reach a characteristic and seq is empty, we go to the left
      return ajouter_espece(&((*a)->gauche), espece, seq);
   } //these last 2 conditions execute the same thing, they could be replaced with an else statement
   assert(0);
}

/* Doit afficher la liste des caractéristiques niveau par niveau, de gauche
 * à droite, dans le fichier fout.
 * Appeler la fonction avec fout=stdin pour afficher sur la sortie standard.
 */
void afficher_par_niveau (arbre racine, FILE* fout) {
   //We implement breadth-first search on the tree to print it
   arbre tree_array[100000];//array for the references to branches we want to print
   tree_array[0] = racine;

   if(racine == NULL){ return; }//we should never be printing and empty tree

   //Initialisation of indexes we use to keep track of the edges of layers
   //in the array of trees.
   int layer_start_index = 0, layer_end_index = 1;
   int next_layer_first_index = 0, next_layer_last_index = 1;

   //initialisation of current, left and right trees for searching;
   arbre curr, curr_g, curr_d;
   //start looping over layers
   while(next_layer_first_index != next_layer_last_index){
      //while size of the layer not 0, i.e. we haven't reached the end of the tree
      next_layer_first_index = layer_end_index;

      for(int i = layer_start_index; i < layer_end_index; i++){
         curr = tree_array[i];

         curr_g = curr->gauche;
         if (curr_g != NULL){
            tree_array[next_layer_last_index] = curr_g;
            next_layer_last_index++;
         }

         curr_d = curr->droit;
         if (curr_d != NULL){
            tree_array[next_layer_last_index] = curr_d;
            next_layer_last_index++;
         }

         if (curr_g != NULL || curr_d != NULL){
            fprintf(fout, "%s ",curr->valeur);
         }
      }
      assert(next_layer_first_index == layer_end_index);
      layer_start_index = layer_end_index;
      layer_end_index = next_layer_last_index;
      fprintf(fout, "\n");
   } 
}

// Acte 4

bool is_in_list(char* animal_name, cellule_t* seq){
   for(cellule_t* i = seq; i != NULL; i = i->suivant){
      if (strcmp(i->val, animal_name) == 0) {return true;}
   }
   return false;
}


arbre* trouver_sous_arbre_despeces(arbre* a, cellule_t* seq){
   //this function returns NULL if there is no subtree containing all and only memebrs of seq
   //returns a reference to NULL if there were no members of seq found in the tree a
   //and returns a refernece to the subtree containing all memebrs of seq (that exist in a) if one is found
   assert(a != NULL);//the pointer we are given should always be valid

   if (*a == NULL){
      //we have reached an empty leaf, we should return a reference to NULL, 
      //since there are no members of seq in an empty tree
      //also returns itself since
      return a;
   } else if (a != NULL && (*a)->droit == NULL && (*a)->gauche == NULL) {
      // a is a leaf, we check if it is in seq
      if ( is_in_list((*a)->valeur, seq) ){
         //a is in seq, we return a refernce to a
         return a;
      } else {
         //a is not in seq, we create and return a refernce to NULL
         arbre* empty = (arbre*) malloc(sizeof(arbre));//this can leak memory, we should free this in the calling function
         *empty = NULL;
         return empty;
      }
   } else if (a != NULL && ((*a)->droit != NULL || (*a)->gauche != NULL)){
      //a contains a characteristic, it is a non-empty branch that has a child somewhere
      //in this case we call our function recursively and use the return value of the functions to determine our return value  
      
      //initialising our arguments, references to the left and right trees
      arbre* abr_gauche = &((*a)->gauche);
      arbre* abr_droit = &((*a)->droit);

      //calling our function recursively
      arbre* res_gauche = trouver_sous_arbre_despeces(abr_gauche, seq);
      arbre* res_droit = trouver_sous_arbre_despeces(abr_droit, seq);

      if (res_gauche == NULL || res_droit == NULL){
         //there was a problem in one of the children, we return a problem
         return NULL;
      } else if (res_gauche == abr_gauche && res_droit == abr_droit) {
         //both recursive calls returned themselves, 
         //a is a subtree containing only members of seq, we should return a
         return a;
      } else if (*res_gauche == NULL){
         //if one of the branches has no elements of seq, 
         //we return the other branch's return value automatically
         //we have already dealt with the case where both return themselves
         //which is the only case where we can incorporate a branch which returns a reference to anempty tree
         return res_droit;
      } else if (*res_droit == NULL){
         return res_gauche;
      } else if (res_gauche != NULL && res_droit != NULL){
         //both children returned nonempty subtrees
         //but one of them contains species not in seq since they didn't both return themselves
         //this means species in seq are not concentrated in a single subtree, which is an error
         return NULL;
      }
      assert(false);
   }
   assert(false);
   return 0;
}

int ajouter_carac(arbre* a, char* carac, cellule_t* seq) {
   assert(a != NULL);

   if (*a == NULL){
      // If a is empty we don't want to add a branch above it 
      //as that would make our characteristic a leaf since both children would be empty
      //and the "characteristic" would be interpreted as a species
      //and so we don not modify a and return 1 immediately
      return 1;
   }

   arbre* ancetre_commun = NULL;//initialise pointer that will point to the desired subtree

   if (seq != NULL){//if there are species that have the caracteristic we search for the subtree 
      ancetre_commun = trouver_sous_arbre_despeces(a, seq);
      if (ancetre_commun == NULL){
         printf("Ne peut ajouter %s: ne forme pas un sous-arbre.", carac);
         return 0;
      }  
   }

   //ancetre_commun is the subtree that contains all the species having the caracteristic
   if (*ancetre_commun == NULL){//There are no species in the tree which are in seq
      arbre new_branch = nouveau_noeud();//setting up new branch to contain the caracteristic
      new_branch->valeur = strdup(carac);//duplicating the string to put in value
      new_branch->gauche = *a;//a is on the left since no species in a has the characteristic 'carac'
      a = &new_branch;//we insert the new caracteristic at the root of the tree 
   }else{
      arbre new_branch = clone_arbre(*ancetre_commun);//create a copy of the root of the subtree
      (*ancetre_commun)->valeur = strdup(carac);//replace the value with carac
      (*ancetre_commun)->droit = new_branch;//make it point to the clone on the right
      (*ancetre_commun)->gauche = NULL;//replace the left pointer with an empty tree
      //ancetre_commun has become the branch with carac, 
      //and the clone has replaced it in the structure of the tree

      //Note that this method of cloning is slightly inefficient 
      //but since our function returns a pointer to this branch, it is the easiest way to edit the tree's structure
   }
   return 1;
}