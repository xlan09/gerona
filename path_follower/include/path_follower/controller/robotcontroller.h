#ifndef ROBOTCONTROLLER_H
#define ROBOTCONTROLLER_H

/// THIRD PARTY
#include <Eigen/Core>

/// PROJECT
#include <path_follower/utils/path.h>
#include <path_follower/obstacle_avoidance/obstacledetector.h>

class PathFollower;

class RobotController
{
    /* DATA */
public:
    enum ControlStatus
    {
        MOVING,
        OBSTACLE,
        SUCCESS,
        ERROR
    };

    /* ABSTRACT METHODS */
public:
    //! Return obstacle detector working with obstacle map. Only used, if ~use_obstacle_map:=true
    virtual ObstacleDetector* getObstacleDetector() = 0;

    virtual void publishCommand() = 0;

    //! Immediatley stop any motion.
    virtual void stopMotion() = 0;

    virtual bool isOmnidirectional() const;

    virtual void start() {}
    virtual ControlStatus execute() {}


    virtual void behaveOnLine() {}

    /**
     * @return True, when turning point is reached, otherwise false.
     */
    virtual bool behaveApproachTurningPoint() {}


    /* REGULAR METHODS */
public:
    RobotController(ros::Publisher &cmd_publisher, PathFollower *path_driver) :
        cmd_pub_(cmd_publisher),
        path_driver_(path_driver),
        velocity_(0.0f),
        filtered_speed_(0.0f),
        dir_sign_(1.0f)
    {
    }

    /* RESET FOR A NEW PATH */
    virtual void reset() {}


    virtual void setPath(PathWithPosition path);

    /* BEHAVIOURS */
    //! Initialize the OnLine-Behaviour
    virtual void initOnLine() {}
    //! Initialize the ApproachTurningPoint-Behaviour
    virtual void initApproachTurningPoint() {}

    virtual void setVelocity(float v)
    {
        ROS_WARN_STREAM("setting velocity to " << v);
        velocity_ = v;
    }

    virtual void setDirSign(float s)
    {
        dir_sign_ = s;
    }

    virtual float getDirSign() const
    {
        return dir_sign_;
    }

protected:
    ros::Publisher& cmd_pub_;

    PathFollower *path_driver_;

    //! Desired velocity (defined by the action goal).
    float velocity_;

    float filtered_speed_;

    //! Indicates the direction of movement (>0 -> forward, <0 -> backward)
    float dir_sign_;

    //! Current subpath.
    PathWithPosition path_;
    //! The next waypoint in the robot frame (set by setPath).
    Eigen::Vector3d next_wp_local_;


    virtual void setFilteredSpeed( const float speed ) {
        filtered_speed_ = speed;
    }

    virtual float getFilteredSpeed() const {
        return filtered_speed_;
    }

    void setStatus(int status);

    //! Calculate the angle between the orientations of the waypoint and the robot.
    virtual double calculateAngleError();
};

#endif // ROBOTCONTROLLER_H