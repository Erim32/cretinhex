/**
 * \file LDC.h
 * \brief Fichier header du type LDC (liste doublement chainée).
 * \author Pierre POMERET-COQUOT
 * \version 1.0
 * \date 1 mars 2017
 *
 *
 * Une LDC bien ordinaire.
 *
 *
 * Type abstrait de données dans le fichier header
 *                
 */



# include <stdlib.h>
# include <stdio.h>
# include <assert.h>
# include "LDC.h"








/**
 * \defgroup pack_cretinlib_LDCCellule LDCCellule
 * \ingroup pack_cretinlib_LDC
 *
 * \par Description
 * LDCCellule est une simple cellule de LDC, connaissant son prédécesseur et son successeur.
 * Elle contient un LDCElement.
 *
 * \par Type abstrait de données
 *
 * <pre style="line-height:1em;">
 * SORTE :            LDCCellule
 * 
 * UTILISE :          LDCElement
 *
 * CONSTRUCTEURS :    LDCCellule_init                                     LDCElement -> LDCCellule    
 *
 * OPERATEURS :       LDCCellule_libererMemoire                           LDCElement -> _____
 *                    LDCCellule_insererEntre     LDCElement, LDCCellule, LDCCellule -> _____
 *
 * Cette structure étant publique, on accède directement à ses valeurs
 * </pre>
 *                    
 * @{
 */

/**
 * \struct LDCCellule
 * \brief Cellule de liste doublement chaînée
 */
typedef struct LDCCellule {
	struct LDCCellule * precedent;
	struct LDCCellule * suivant;
	LDCElement valeur;
	LDCElementFree free;
} * LDCCellule;







/**
 * \brief Alloue de la mémoire pour une LDCCellule
 * \param e L'élément de la cellule (ou NULL pour laisser vide)
 *   - une fonction de profil LCDElementFree si la LDC parente gère exclusivement 
 *     une zone de mémoire allouée dynamiquement
 *   - NULL si la LDC parente gère des variables déclaré dans un scope identique ou plus large
 * \param free La méthode pour supprimer cet élément
 * \return La LDCCellule
 *
 *
 * LDCCellule_init crée un LDCCellule dont la valeur pointe vers le LDCElement e
 * Les champs precedent et suivant pointent sur elle-même
 */
LDCCellule LDCCellule_init(LDCElement e, LDCElementFree free){
	LDCCellule c = (LDCCellule) malloc(sizeof(struct LDCCellule));
	assert(c != NULL);
	c->precedent = c;
	c->suivant = c;
	c->valeur = e;
	c->free = free;
	return c;
}



/**
 * \fn void LDCCellule_libererMemoire(LDCCellule c)
 * \brief Libère la mémoire allouée à une LDCCellule
 * \param c La cellule à supprimer
 *
 */
void LDCCellule_libererMemoire(LDCCellule * c){
	if ((*c)->free != NULL)
		(*c)->free(&((*c)->valeur));
	free(*c);
	*c = NULL;
}



/**
 * \brief Insère une valeur entre deux cellules
 * \param e L'élément à inserer
 * \param free La méthode pour supprimer cet élément
 * \param c1 La cellule précédente
 * \param c2 La cellule suivante
 *
 */
void LDCCellule_insererEntre(LDCElement e, LDCElementFree free, LDCCellule c1, LDCCellule c2){
	assert(c1->suivant == c2);
	assert(c2->precedent == c1);
	LDCCellule c = LDCCellule_init(e, free);
	c->precedent = c1;
	c1->suivant = c;
	c->suivant = c2;
	c2->precedent = c;
}


/**
 * \brief Donne la cellule précédente
 */
LDCCellule LDCCellule_precedent(LDCCellule c){
	return c->precedent;
}


/**
 * \brief Donne la cellule suivante
 */
LDCCellule LDCCellule_suivant(LDCCellule c){
	return c->suivant;
}



/** @} */












typedef struct LDC {
	LDCCellule sentinelle;
	int taille;
} LDCInterne;




/*
 * \fn LDC LDC_init()
 * \brief Alloue de la mémoire pour une LDC
 * \return La LDC
 *
 */
LDC LDC_init(){
	LDC ldc = (LDC) malloc(sizeof(LDCInterne));
	assert(ldc != NULL);
	ldc->taille = 0;
	ldc->sentinelle = LDCCellule_init(NULL, NULL);
	return ldc;
}



/*
 * \fn LDCCellule LDC_obtenurCellule(LDC ldc, int pos)
 * \brief Renvoie la LDCCellule à cette position
 * \return La LDCCellule
 *
 * La LDC ne peut pas être vide.
 * Son premier élément est à la position 0
 *
 */
LDCCellule LDC_obtenirCellule(LDC ldc, int pos){
	assert(ldc->taille > 0);
	int i;
	LDCCellule c = ldc->sentinelle;
	if (pos >= 0)
		for (i = -1; i < pos; ++i)
			c = c->suivant;
	else
		for (i = 0; i > pos; --i)
			c = c->precedent;
	return c;
}
	


/*
 * \fn LDC LDC_insererElement(LDC ldc, int pos, LDCElement e, LDCElementFree free)
 * \brief Insère l'élément e à la position voulue
 * \return La LDC
 *
 */
LDC LDC_insererElement(LDC ldc, int pos, LDCElement e, LDCElementFree free){
	assert((pos >= 0 && pos <= ldc->taille) || (pos < 0 && pos >= -(ldc->taille) - 1));
	if (ldc->taille == 0)
		LDCCellule_insererEntre(e, free, ldc->sentinelle, ldc->sentinelle);
	else {
		LDCCellule c = LDC_obtenirCellule(ldc, pos);
		if (pos >= 0)
			LDCCellule_insererEntre(e, free, c->precedent, c);
		else
			LDCCellule_insererEntre(e, free, c, c->suivant);
	}
	++ldc->taille;
	return ldc;
}



/*
 * \fn LDCElement LDC_obtenirElement(LDC ldc, int pos)
 * \brief Insère l'élément e à la position voulue
 * \return La LDC
 *
 */
LDCElement LDC_obtenirElement(LDC ldc, int pos){
	LDCCellule c = LDC_obtenirCellule(ldc, pos);
	return c->valeur;
}



/*
 * \fn LDC LDC_enleverElement(LDC ldc, int pos)
 * \brief Insère l'élément e à la position voulue
 * \return La LDC
 *
 */
LDC LDC_enleverElement(LDC ldc, int pos){
	LDCCellule c = LDC_obtenirCellule(ldc, pos);
	c->precedent = c->suivant;
	c->suivant = c->precedent;
	LDCCellule_libererMemoire(&c);
	return ldc;
}



/*
 * \fn int LDC_taille(LDC ldc)
 * \brief Insère l'élément e à la position voulue
 * \return La LDC
 *
 */
int LDC_taille(LDC ldc){
	return ldc->taille;
}



/*
 * \fn void LDC_libererMemoire(LDC ldc)
 * \brief Libère la mémoire allouée à une LDC
 * \param ldc La LDC à supprimer
 *
 */
void LDC_libererMemoire(LDC * ldc){
	LDCCellule c = (*ldc)->sentinelle->suivant;
	while (c != (*ldc)->sentinelle){
		c = c->suivant;
		LDCCellule_libererMemoire(&(c->precedent));
	} 
	LDCCellule_libererMemoire(&c);
	free(*ldc);
	*ldc = NULL;
}
















/*
 * \struct LDCIterateur
 * \brief Itérateur pour la LDC
 */
typedef struct LDCIterateur {
	LDC ldc;
	LDCCellule c;
	LDCCellule (*avancer)(LDCCellule);
	int position;
} LDCIterateurInterne;

/*
 * \fn LDCIterateur LDCIterateur_init(LDC ldc, sens)
 * \brief Instancie un itérateur
 * \param ldc La LDC sur laquelle itérer
 * \return L'itérateur
 */
LDCIterateur LDCIterateur_init(LDC ldc, int sens){
	assert(sens == LDCITERATEUR_AVANT || sens == LDCITERATEUR_ARRIERE);
	
	LDCIterateur it = (LDCIterateur) malloc(sizeof(LDCIterateurInterne));
	assert(it != NULL);
	
	it->ldc = ldc;
	if (sens == LDCITERATEUR_AVANT)
		it->avancer = LDCCellule_suivant;
	else 
		it->avancer = LDCCellule_precedent;
	return it;
}

/*
 * \fn void LDCIterateur_libererMemoire(LDCIterateur * it)
 * \brief Libère la mémoire allouée à un itérateur
 * \param it Pointeur vers un itérateur
 */
void LDCIterateur_libererMemoire(LDCIterateur * it){
	free(*it);
	*it = NULL;
}

/*
 * \fn LDCIterateur LDCIterateur_avancer()
 * \brief Avance 
 */
LDCIterateur LDCIterateur_avancer(LDCIterateur it){
	it->c = it->avancer(it->c);
	++ it->position;
	return it;
}


/*
 * \fn LDCIterateur LDCIterateur_debut(LDCIterateur it)
 * \brief Amène l'itérateur au début de la LDC
 */
LDCIterateur LDCIterateur_debut(LDCIterateur it){
	it->c = it->avancer(it->ldc->sentinelle);
	it->position = 0;
	return it;
}


/*
 * \fn int LDCIterateur_fin(LDCIterateur it)
 * \brief Détermine si l'itérateur est arrivé à la fin de la ldc
 * \param it L'itérateur
 * \return Vrai si l'itérateur est hors limite
 */
int LDCIterateur_fin(LDCIterateur it){
	return (it->c == it->ldc->sentinelle);
}


/*
 * \fn LDCElement LDCIterateur_valeur(LDCIterateur it)
 * \brief Donne la valeur à la position de l'itérateur
 */
LDCElement LDCIterateur_valeur(LDCIterateur it){
	return it->c->valeur;
}


/*
 * \fn int LDCIterateur_position(LDCIterateur it)
 * \brief Donne la valeur à la position de l'itérateur
 */
int LDCIterateur_position(LDCIterateur it){
	return it->position;
}

