using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Majestic : MonoBehaviour
{
    public float moveSpeed = 0; //public variables may be changed in the inspector 
    private float gravity = 3;

    private CharacterController controller;

    private Vector3 moveDirection = Vector3.zero;

    private void Start()
    {
        controller = GetComponent<CharacterController>(); //srsly idk!!

    }

    private void Update()
    {
        if (controller.isGrounded) // can only move if grounded 
        {
            float moveX = Input.GetAxis("Horizontal");
            float moveZ = Input.GetAxis("Vertical");

            moveDirection = new Vector3(moveX, 0, moveZ);// Oh no
            moveDirection *= moveSpeed;
        }
        moveDirection.y -= gravity; //gravity 

        controller.Move(moveDirection * Time.deltaTime); //What the hell?
    }
}
