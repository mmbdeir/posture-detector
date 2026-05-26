# posture-detector
We built a real time posture and movement detector that can make a post or get request containing the results.

The motive behind this project was to detect the posture and movement of hospital patients locally without needing to store footage while protecting privacy.

The camera used is a Huskylens-v2 that is connected to an arduino. The arduino runs the **project.ino** file to receive the location of the joints detected on the Huskylens and estimates the persons posture and movement to return one of the following: "standing", "sitting", "laying down". The result is then sent to a definable endpoint.


