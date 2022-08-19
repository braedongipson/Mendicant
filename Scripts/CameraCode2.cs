using System.Collections;
using System.Collections.Generic;
using UnityEngine;
public class CameraCode2 : MonoBehaviour
{
    [SerializeField] Transform cam;
    public Transform targetObject;
    /* ublic Transform targetObject1;
    public Transform targetObject2; */
    Vector3 cameraDirection;
    public float camDistance;
    public float smoothness;
   // public Vector3 height;// = new Vector3(0,0.1f,0);
    public Vector2 cameraDisMinMax;// = new Vector2(0.5f, 5f);
    // Start is called before the first frame update
    void Start()
    {
        cameraDirection = transform.localPosition.normalized;
        camDistance = cameraDisMinMax.y;
    }

    // Update is called once per frame
    void Update()
    {
        occlusionCollisionCheck(cam);
    }
    public void occlusionCollisionCheck(Transform cam){
        //Vector3 desiredCamPosition = transform.TransformPoint(cameraDirection*3);
        RaycastHit hit;
        Debug.DrawLine(cam.transform.position, targetObject.position, Color.red);
        if(Physics.Linecast(cam.transform.position, targetObject.position, out hit)){
            camDistance = Mathf.Clamp(hit.distance*0.5f, cameraDisMinMax.x, cameraDisMinMax.y);
        }else{
            camDistance = cameraDisMinMax.y;
        }
        //cameraDirection += height;
        //cam.localPosition = Vector3.Lerp(cam.localPosition, cameraDirection* camDistance, smoothness*Time.fixedDeltaTime);//interpolation not working 
       // cam.localPosition += height;
        
        //transform.LookAt(targetObject.transform);
    }

    /* bool linecasts(){
        RaycastHit hit;
        bool[] arr = new bool[3];
        Debug.DrawLine(cam.transform.position, targetObject.position, Color.red);
        arr[0] = Physics.Linecast(cam.transform.position, targetObject.position, out hit);
        return false;
    }
*/
}





