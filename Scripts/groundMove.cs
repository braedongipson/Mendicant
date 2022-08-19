using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class groundMove : MonoBehaviour
{
    // Start is called before the first frame update
    public float min;
    public float max;
    // Use this for initialization
    void Start () {
        min=transform.position.x-10;
        max=transform.position.x+10;
    }
   
    // Update is called once per frame
    void Update () {
       
        transform.position =new Vector3(Mathf.PingPong(Time.time*5,max-min)+min, transform.position.y, transform.position.z);
       
    }
}
