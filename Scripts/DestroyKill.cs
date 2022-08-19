using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class DestroyKill : MonoBehaviour
{
    GameObject child;
    public GameManager gameManager;
    //basically this script will destroy the player object if it collides with anything tagged with "DestroyKill"
    void OnCollisionEnter(Collision other)
    {
        if(other.transform.tag == "DestroyKill")
        {  
            child = gameObject.transform.GetChild(12).gameObject;//specifically for the current camera set up
            child.transform.parent = null;//specifically for the current camera set up
            for(int i = 0; i<12; i++){
                Destroy(gameObject.transform.GetChild(i).gameObject);
            }
           //WaitForSeconds seconds = new WaitForSeconds(30f);
           Invoke("nothing", 50f);
            gameManager.EndGame();
            //Destroy(gameObject);
            //gameManager.CompleteLevel();
            //
        }
    }

   void OnCollisionExit(Collision other)
    {

    }

    void nothing(){

    
    }

}
