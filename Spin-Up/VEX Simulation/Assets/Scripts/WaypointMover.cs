using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class WaypointMover : MonoBehaviour
{
    [SerializeField] private Waypoints waypoints;

    [SerializeField] private float moveSpeed = 5f;

    [SerializeField] private float distanceThreshold = 1.5f; // distance constant

    private Transform currentWaypoint;

    // Start is called before the first frame update
    void Start()
    {
        // Set initial position for first waypoint
        currentWaypoint = waypoints.GetNextWaypoint(currentWaypoint);
        transform.position = currentWaypoint.position;

        // Move to next waypoint
        currentWaypoint = waypoints.GetNextWaypoint(currentWaypoint);
        transform.LookAt(currentWaypoint);
    }

    // Update is called once per frame
    void Update()
    {
        // Continuously move toward the next waypoint
        transform.position = Vector3.MoveTowards(transform.position, currentWaypoint.position, moveSpeed * Time.deltaTime);
        Debug.Log(Vector3.Distance(transform.position, currentWaypoint.position));
        // Change next waypoint if objects get close to its target
        if (Vector3.Distance(transform.position, currentWaypoint.position) < distanceThreshold){
            currentWaypoint = waypoints.GetNextWaypoint(currentWaypoint);
            transform.LookAt(currentWaypoint);
        }
    }
}