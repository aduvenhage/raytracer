# Job System

## Notes on Performance
Raytracing can typically be distributed very effectively, since the rendering of a pixel does not depend on any other pixel.  It is relatively easy to run the raytracer accross multiple threads:

<p float="left">
  <img src="./images/do_32_cores.png" alt="Raytracing using near 100% of CPU" width="100%"/>
</p>

For raytracing the workload is CPU bound and it is normally a good idea then to have twice as many worker threads as CPU cores.


## The Job System
The current implementation renders lines in the output image concurrently, where each line is packaged as a 'job' and the jobs are executed by several worker threads.  Having twice as many worker threads as CPU cores provides good results in most cases.

The job system has three parts:
- the job base class: a job represents a task or piece of work that would be executed on its own; you have to overload the base class to create something that can be executed as a piece of work.
- job queue: a thread-safe queue that contains all of the job instances to process.
- the worker: a generic worker thread that operates on the job queue; it pops a job off the queue, executes it and then moves on to the next job; the job system would have one or more workers that operate on the job queue; if there are no jobs in the queue, the worker thread would just wait until there is something in the queue.

## Pixel Jobs
The job interface is extended to implement pixel jobs, where each pixel job instance is responsible for rendering a single full line in the output image.  There are then as many jobs as output lines in the image, for example, 768 in a 1024x768 output image.


## Running to Completion and Job Stats
The job system (workers, queue & jobs) is integrated into the raytracer's frame class and all jobs in a specific system only contribute to a single frame.  
The job base class is generic and does not support any rendering stats, like ray count. It only supports progress feedback.  
To get the stats, all pixel jobs reference a frame stats object and the jobs report on this frame stats object.
For example, each job would report the number of rays it created after it is done, which then contributes the the `total rays` and `rays per second` stats for the frame.

The frame also keeps track of the number of completed jobs and job progress and can report on the overall frame progress.
