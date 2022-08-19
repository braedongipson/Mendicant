using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Movement : MonoBehaviour
{

    public float speed = 10f;
    //public Rigidbody rb;
    
    Rigidbody rb;

    [SerializeField] Transform groundCheck;
    [SerializeField] LayerMask ground;

    //REFERENCES
    private Animator anim;
    Vector3 m_EulerAngleVelocity;
    Vector3 m_EulerAngleVelocity1;
    private void Start()
    {
        rb = GetComponent<Rigidbody>();
        anim = GetComponentInChildren<Animator>();
        m_EulerAngleVelocity = new Vector3(0, 20, 0);
    }


    // Update is called once per frame
    //void FixedUpdate()
    void Update()
    {
        float horizontal = Input.GetAxis("Horizontal")*speed*Time.deltaTime; //what is time.deltaTime
        float vertical = Input.GetAxis("Vertical")*speed*Time.deltaTime;
        //add if(isGrounded) check here
        rb.velocity = new Vector3(horizontal, rb.velocity.y, vertical);
        anim.SetBool("onGround", false);

       
        
            if (vertical == 0 && horizontal == 0)
            {
                anim.SetFloat("Speed", 0);
            }else{
                
                transform.Translate(horizontal, 0, vertical);
               /* 
                transform.rotation = Quaternion.Slerp(from.rotation, to.rotation, timeCount);
                timeCount = timeCount + timeCount.deltaTime;
                rb.MoveRotation(rb.rotation * transform.rotation); */

                if(Input.GetKey("right")){
                    Quaternion deltaRotation = Quaternion.Euler(m_EulerAngleVelocity * Time.fixedDeltaTime);
                    rb.MoveRotation(rb.rotation * deltaRotation);

                }else if(Input.GetKey("left")){
                    Quaternion deltaRotation1 = Quaternion.Euler(m_EulerAngleVelocity * -Time.fixedDeltaTime);
                    rb.MoveRotation(rb.rotation * deltaRotation1);
                }
                

                anim.SetFloat("Speed", 0.5f);
            }
            

            if (Input.GetButtonDown("Jump") && IsGrounded()) // I cant seem to properly animate the movement for this one.
            {
            anim.SetFloat("Speed", 1);
            //aargghh!! half the time I press the space button it will not jump and i have no clue why
            rb.velocity = new Vector3(rb.velocity.x, 15f, rb.velocity.z);

                //rb.AddForce(new Vector3(0, 5, 0), ForceMode.Impulse); //whhhhhyyyyyyyy
                anim.SetBool("onGround", false);
            } else{
                anim.SetBool("onGround", true);
            }
            
    }
    bool IsGrounded()
    { //ground check 
        return Physics.CheckSphere(groundCheck.position, 1f, ground);
    }



}


