using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class EndTrigger : MonoBehaviour
{
    public GameObject Player;

    public GameManager gameManager;

    private void OnTriggerEnter(Collider other)
    {
        if (other.gameObject == Player)
        {
            gameManager.CompleteLevel();
        }
       
    }
}
