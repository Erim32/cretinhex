/**
 * \class     noyau.BridgeBot
 * \brief     IA détecte les ponts, et essaie de fermer les siens en empèchant l'adversaie de le faire.
 * \author    Pierre Pomeret-Coquot
 * \date      14 avril 2017
 */
package noyau;
import cretinplay.Application;

public class BridgeBot implements Utilisateur {


	private String nom;     /**< \brief Nom de l'IA*/	
	private Joueur j;       /**< \brief Joueur joué par l'IA (J1 ou J2) */	
	
	/** \brief Charge la bibliothèque cretin_bridgebot */
	static { System.loadLibrary("cretinhex_bridgebot"); }


	/** \brief Constructeur d'après le joueur en question (J ou J2)*/
	public BridgeBot(Joueur j){
		this.nom = "BridgeBot";
		bridgebotXinit(j.toInt());
	}
	
	
	/** \brief Appelle queJouer par JNI */
	private native int[] bridgebotXjouer(int[] damier, int tailleDamier);
	
	/** \brief Alloue de la mémoire du bot */
	private native void bridgebotXinit(int j);
	
	/** \brief Libère la mémoire allouée au bot */
	private native void bridgebotXlibererMemoire();
	
	

	public int[] jouer(Partie p){
		return bridgebotXjouer(p.obtenirDamierInt(), p.largeurDamier());
	}
	
	public void actualiser(int x, int y){
	}
	
	public void quitter(){
		bridgebotXlibererMemoire();
	}


	public String getNom(){
		return this.nom;
	}
	
	public void setNom(String nom){
		this.nom = nom;
	}
	
}
