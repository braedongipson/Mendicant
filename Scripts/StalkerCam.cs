using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class StalkerCam : MonoBehaviour
{
    
    public float smoothness;
    public Transform targetObject; //set target to brow.B.R.001 (Transform)
    private Vector3 initialOffset;
    private Vector3 cameraPosition;
    
    //private Quaternion cameraRotation;
    // Start is called before the first frame update
    void Start()
    {
        // initialOffset = transform.position - targetObject.position;
        initialOffset = targetObject.transform.position - transform.position;
    }

    // Update is called once per frame
    void LateUpdate()
    {   //Quaternion deltaRotation1 = Quaternion.Euler(0, 90, 0);
        //cameraPosition = targetObject.position + initialOffset;
        //transform.position = Vector3.Lerp(transform.position, cameraPosition, smoothness * Time.fixedDeltaTime);
        float currentAngle = transform.eulerAngles.y;
        float desiredAngle = targetObject.transform.eulerAngles.y;
        float angle = Mathf.LerpAngle(currentAngle, desiredAngle, Time.fixedDeltaTime);

        Quaternion rotation = Quaternion.Euler(0, desiredAngle, 0); //creates the desired angle

        transform.position = targetObject.transform.position - (rotation * initialOffset); //i'm scared please hold me 
      
        transform.LookAt(targetObject.transform);

    }
}
