/**
 * \file Graphe.c
 * \brief Implémentation du type Graphe.
 * \author Pierre POMERET-COQUOT
 * \version 1.0
 * \date 3 avril 2017
 *
 */
 
# include "Graphe.h"
# include <stdio.h>
# include <stdlib.h>
# include <assert.h>



/*
 * \defgroup pack_cretinlib_Graphe Graphe
 * \ingroup pack_cretinlib
 * \author Pierre POMERET-COQUOT
 * \version 1.0
 * \date 3 avril 2017
 *
 * \par Description
 * \todo Rapide description
 *
 *
 * \par Type abstrait de données
 *
 * <pre style="line-height:1.1em;">
 * SORTE :            Graphe
 * 
 * UTILISE :          GrapheElement, LDC, int
 *
 * CONSTRUCTEURS :    Graphe_init                                       int[] -> Graphe

 * \todo Finir le TAD     
 *
 * @{               
 */


typedef struct Graphe {
	LDC pointsEntree;
	int nbNoeuds;
} GrapheInterne;














/*
 * \defgroup pack_cretinlib_GrapheNoeud GrapheNoeud
 * 
 * \par Description
 * Type manipulé par les Graphes, dans lesquels vous pouvez stocker vos &laquo; trucs &raquo;
 *
 * \par Type abstrait de données
 * <pre style="line-height: 1em;">
 *
 * Sorte :           GrapheNoeud
 *
 * Utilise :         GrapheElement (void *)
 *                   GrapheElementFree (fonction supprimant l'élément)
 *
 * Constructeur :    init :                GrapheElement, GrapheElementFree → GrapheNoeud
 *
 * Opérateurs :      obtenirElement :                           GrapheNoeud → GrapheElement
 *                   obtenirVoisins :                           GrapheNoeud → LDC (GrapheNoeud[])
 *                   fusionner :                  GrapheNoeud x GrapheNoeud → GrapheNoeud
 *                   libererMemoire :                           GrapheNoeud → __
 *
 * </pre>
 * 
 * @{               
 */

typedef struct GrapheNoeud {
	GrapheElement element;
	GrapheElementFree free;
	LDC voisins;
} GrapheNoeudInterne;




/*
 * \brief   Initialise un GrapheNoeud
 * \param   e L'élément contenu dans ce noeud
 * \param   free Fonction pour effacer l'élément, ou NULL
 * \return  Le noeud initialisé
 */
GrapheNoeud GrapheNoeud_init(GrapheElement e, GrapheElementFree free){
	GrapheNoeud noeud;
	
	noeud = (GrapheNoeud) malloc(sizeof(GrapheNoeudInterne));
	assert(noeud != NULL);
	
	noeud->element = e;
	noeud->free = free;
	noeud->voisins = LDC_init();
	
	return noeud;
}

void GrapheNoeud_afficher(GrapheNoeud n){
/*	const char * espaces = "              "; */
	printf("GrapheNoeud : %4d (élément %4d)\n", (int) (unsigned long int) n % 10000, (int) (unsigned long int) n->element % 10000);
}


/*
 * \brief Égalité de noeuds : même adresse
 */
int GrapheNoeud_estEgal(GrapheNoeud n1, GrapheNoeud n2){
	return (n1 == n2);
}



/*
 * \brief Ajoute un voisin et réciproquement.
 * \note  A double sens, modifie le voisin par effet de bord
 */
GrapheNoeud GrapheNoeud_ajouterVoisin(GrapheNoeud noeud, GrapheNoeud voisin){
	printf("ajouterVoisin : @ %d <--> @ %d\n", (int)(unsigned long int) noeud % 10000, (int) (unsigned long int) voisin % 10000);
	if (LDC_obtenirPosition(noeud->voisins, voisin, (LDCElementEgal) GrapheNoeud_estEgal) < 0){
		noeud->voisins = LDC_insererElement(noeud->voisins, -1, voisin, NULL);
		voisin->voisins = LDC_insererElement(voisin->voisins, -1, noeud, NULL);
	}
	return noeud;
}


/*
 * \brief   Récupère l'élément contenu dans un noeud
 */
GrapheElement GrapheNoeud_obtenirElement(GrapheNoeud noeud){
	return noeud->element;
}


/*
 * \brief   Donne la liste des noeuds voisins
 */
LDC GrapheNoeud_obtenirVoisins(GrapheNoeud noeud){
	return noeud->voisins;
}



/*
 * \brief   Fusionne deux noeuds dans le premier
 * \param   n1 Le premier noeud
 * \param   n2 Le noeud à fusionner dans le premier
 * \return  Le premier noeud fusionné avec le second
 */
GrapheNoeud GrapheNoeud_fusionner(GrapheNoeud n1, GrapheNoeud n2){
	GrapheNoeud n3;
	int i;
	LDCIterateur it;
	
	
	/* Pour chacun des voisins de n2 */
	it = LDCIterateur_init(n2->voisins, LDCITERATEUR_AVANT);
	for (it = LDCIterateur_debut(it); ! LDCIterateur_fin(it); it = LDCIterateur_avancer(it)){
		n3 = LDCIterateur_valeur(it);
		
		/* On débranche n2 : n3 ne le voit plus */
		i = LDC_obtenirPosition(n3->voisins, n2, (LDCElementEgal) GrapheNoeud_estEgal);
		assert(i >= 0);
		n3->voisins = LDC_enleverElement(n3->voisins, i);
		
		/* Crée un lien entre n1 et n3, si absent */
		n1 = GrapheNoeud_ajouterVoisin(n1, n3);
	}
	
	/* n2 est seul, on le supprime */
	GrapheNoeud_libererMemoire(&n2);
	
	LDCIterateur_libererMemoire(&it);
	
	return n1;
}


/*
 * \brief   Libère la mémoire allouée à un noeud
 * \note    Libère aussi la mémoire allouée à l'élément avec la fonction GrapheElementFree fournit à l'initialisation, si différente de NULL
 */
void GrapheNoeud_libererMemoire(GrapheNoeud * noeud){
	if ((*noeud)->free != NULL)
		(*noeud)->free(&((*noeud)->element));
	LDC_libererMemoire(&((*noeud)->voisins));
	free(*noeud);
}


 
/** @} */











/* Graphe */

/*
 * \brief   Initialise un graphe
 * \param   pointsEntree    Un tableau contenant les noeuds qui sont des points d'entrées
 * \param   nbPointsEntree  La taille de ce tableau
 * \return  Un graphe initialisé
 */
Graphe Graphe_init(int nbPointsEntree){
	Graphe g;
	int i;

	g = (Graphe) malloc(sizeof(GrapheInterne));
	assert(g != NULL);

	g->pointsEntree = LDC_init();
	for (i = 0; i < nbPointsEntree; ++i)
		g->pointsEntree = LDC_insererElement(g->pointsEntree, -1, GrapheNoeud_init(NULL, NULL), (LDCElementFree) GrapheNoeud_libererMemoire);
	
	g->nbNoeuds = 0;
	
	
	return g;
}

void Graphe_afficher(Graphe g){
	const char * espaces = "         ";
	
	/* Points d'entrée */
	printf("Graphe : %d point(s) d'entrée : ", Graphe_nbPointsEntree(g));
	LDC_afficher(g->pointsEntree);
	
	printf("%s%d noeud(s) ", espaces, g->nbNoeuds);
	LDC_afficher(Graphe_tousLesNoeuds(g));
}

/*
 * \brief Donne le nombre de points d'entrées (passé lors de l'initialisation)
 */
int Graphe_nbPointsEntree(Graphe g){
	return LDC_taille(g->pointsEntree);
} 

/*
 * \brief   Récupère un point d'entrée (un noeud sans valeur)
 * \param   g Le graphe en question
 * \param   i L'indice du point d'entrée
 * \return  Le point d'entrée demandé
 * \req     0 &le; i &lt; Graphe_nbPointsEntree(g)
 */
GrapheNoeud Graphe_pointEntree(Graphe g, int i){
	return (GrapheNoeud) LDC_obtenirElement(g->pointsEntree, i);
}


/**
 * \brief  Dit si un noeud est un point d'entrée
 */
int GrapheNoeud_estPointEntree(Graphe g, GrapheNoeud n){
	LDCIterateur it;
	int oui = 0;
	it = LDCIterateur_init(g->pointsEntree, LDCITERATEUR_AVANT);
	for(it = LDCIterateur_debut(it); !LDCIterateur_fin(it) && !oui; it = LDCIterateur_avancer(it))
		oui =  (n == (GrapheNoeud) LDCIterateur_valeur(it));
	return oui;
}

/*
 * \brief   Récupère tous les points d'entrée
 * \param   g Le graphe en question
 * \param   i L'indice du point d'entrée
 * \return  Tous les points d'entrées
 * \req     
 */
LDC Graphe_pointsEntree(Graphe g, int i){
	return g->pointsEntree;
}


/*
 * \brief   Renvoie tous les noeuds du graphe
 * \param   g    Le graphe à fouiller
 * \return  Une LDC contenant tous les noeuds du graphe (mais pas les points d'entrée)
 */
LDC Graphe_tousLesNoeuds(Graphe g){
	LDC ldc;
	LDCIterateur it1, it2;
	GrapheNoeud n1, n2;
	int ok;
	
	ldc = LDC_init();
	
	/* On ajoute les voisins du point d'entrée 0 */
	n1 = (GrapheNoeud) LDC_obtenirElement(g->pointsEntree, 0);
	it1 = LDCIterateur_init(n1->voisins, LDCITERATEUR_AVANT);
	for (it1 = LDCIterateur_debut(it1); !LDCIterateur_fin(it1); it1 = LDCIterateur_avancer(it1)){
		n2 = (GrapheNoeud) LDCIterateur_valeur(it1);
		if (LDC_obtenirPosition(ldc, n2, (LDCElementEgal) GrapheNoeud_estEgal) < 0)
			ldc = LDC_insererElement(ldc, -1, n2, NULL);
	}
	LDCIterateur_libererMemoire(&it1);
	
	
	/* On parcourt la liste des voisins et on ajoute les voisins des voisins */
	it1 = LDCIterateur_init(ldc, LDCITERATEUR_AVANT);
	for (it1 = LDCIterateur_debut(it1); !LDCIterateur_fin(it1); it1 = LDCIterateur_avancer(it1)){
	
		printf("ldc : ");
		LDC_afficher(ldc);
	
		ok = 1;
	
		n1 = (GrapheNoeud) LDCIterateur_valeur(it1);
		
		/* Si ce n'est pas un point d'entrée */
		it2 = LDCIterateur_init(g->pointsEntree, LDCITERATEUR_AVANT);
		for (it2 = LDCIterateur_debut(it2); !LDCIterateur_fin(it2); it2 = LDCIterateur_avancer(it2))
			if (n1 == LDCIterateur_valeur(it2))
				ok = 0;
		
		if (ok && LDC_obtenirPosition(ldc, n1, (LDCElementEgal) GrapheNoeud_estEgal) < 0)
			ldc = LDC_insererElement(ldc, -1, n1, NULL);
	}
	LDCIterateur_libererMemoire(&it1);
	
	return ldc;
}


/*
 * \brief   Trouve un noeud dont l'élément est égal à celui recherché
 * \param   g    Le graphe à fouiller
 * \param   e    L'élément à comparer
 * \param   egal Fonction pour comparer les éléments
 * \return  Le noeud trouvé, ou NULL
 */
GrapheNoeud Graphe_trouverNoeud(Graphe g, GrapheElement e, GrapheElementEgal egal){
	int trouve;
	LDC tousLesNoeuds;
	LDCIterateur it;
	GrapheNoeud n;
	
	trouve = 0;
	tousLesNoeuds = Graphe_tousLesNoeuds(g);
	
	it = LDCIterateur_init(tousLesNoeuds, LDCITERATEUR_AVANT);
	
	printf("trouverNoeud : dans ");
	LDC_afficher(tousLesNoeuds);
	
	for (it = LDCIterateur_debut(it); !LDCIterateur_fin(it) && !trouve; it = LDCIterateur_avancer(it)){
		n = (GrapheNoeud) LDCIterateur_valeur(it);
		printf("trouverNoeud : voisin %d @ %d\n", *(int*) ((GrapheNoeud) n)->element, (int) (unsigned long int) n % 10000);
		trouve = egal(n->element, e);
	}
	if (!trouve)/*
		n = (GrapheNoeud) LDCIterateur_valeur(it);
	else*/
		n = NULL;
	printf("trouverNoeud : trouve=%d  renvoi=%d\n", trouve, (int) (unsigned long int) n % 10000);
	LDCIterateur_libererMemoire(&it);
	return n;
}

/*
 * \brief   Insertion d'un noeud
 * \param   g       Le graphe à modifier
 * \param   noeud   Le noeud à insérer, correctement initialisé
 * \param   voisins Un tableau de noeuds alloués appartenant à g : les voisins de 'noeud'
 * \return  Le graphe mis à jour
 * \note    La fonction agit par effet de bord et connect le noeud aux voisins
 * \req     le noeud et les voisins doivent appartenir au graphe (non vérifié)
 */
Graphe Graphe_insererNoeud(Graphe g, GrapheNoeud noeud, GrapheNoeud * voisins, int nbVoisins){
	int i;
	printf("%d voisin(s) dans @%d\n", nbVoisins, (int) (unsigned long int) voisins % 10000);
	for (i = 0; i < nbVoisins; ++i){
		printf("insererNoeud %d @%d\n", i, (int) (unsigned long int) voisins[i] % 10000);
		assert(voisins[i] != NULL);
		noeud = GrapheNoeud_ajouterVoisin(noeud, voisins[i]);
		LDC_afficher(noeud->voisins);
	}
	++g->nbNoeuds;
	return g;
}

/*
 * \brief   Libère la mémoire allouée à un Graphe
 */
void Graphe_libererMemoire(Graphe * g){
	GrapheNoeud n1, n2;
	int i, nbPointsEntree;
	
	/* Fusion : il n'en restera qu'un ! (en fait les n points d'entrée...) */
	n1 = Graphe_pointEntree(*g, 0);
	nbPointsEntree = Graphe_nbPointsEntree(*g);
	while (LDC_taille(n1->voisins) > nbPointsEntree - 1){
		i = 0;
		n2 = (GrapheNoeud) LDC_obtenirElement(n1->voisins, i);
		while (LDC_obtenirPosition((*g)->pointsEntree, n2, (LDCElementEgal) GrapheNoeud_estEgal)){
			++i;
			n2 = (GrapheNoeud) LDC_obtenirElement(n1->voisins, i);
		}
		n1 = GrapheNoeud_fusionner(n1, n2);
	}	
	
	LDC_libererMemoire(&((*g)->pointsEntree));
	free(*g);
	*g = NULL;
}




/** @} */
