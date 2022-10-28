using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Waypoints : MonoBehaviour
{
    [Range(0f, 10f)]
    [SerializeField] private float waypointSize = 1f;

    private void OnDrawGizmos() {
        // Illustrate each child node as a blue sphere
        foreach(Transform t in transform){
            Gizmos.color = Color.blue;
            Gizmos.DrawWireSphere(t.position, waypointSize);
        }

        // Draw a line between each child node
        Gizmos.color = Color.red;
        for (int i = 0; i < transform.childCount - 1; i++){
            Gizmos.DrawLine(transform.GetChild(i).position, transform.GetChild(i + 1).position);
        }
        Gizmos.DrawLine(transform.GetChild(transform.childCount - 1).position, transform.GetChild(0).position);
    }

    // Return the next waypoint
    public Transform GetNextWaypoint(Transform currentWaypoint){
        if (currentWaypoint == null){
            return transform.GetChild(0);
        }

        if (currentWaypoint.GetSiblingIndex() < transform.childCount - 1){
            return transform.GetChild(currentWaypoint.GetSiblingIndex() + 1);
        }else{
            return transform.GetChild(0);
        }
    }
}
