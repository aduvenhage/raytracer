import logging
import time
import click

# https://github.com/aduvenhage/docker-machine-api
from docker_machine_api.cl_api import DockerMachine
from docker_machine_api.machine_config import DigitalOceanConfig
from docker_machine_api.machine_config import AwsConfig


def start_render_machine(scenario):
    # try to create a config
    config = DigitalOceanConfig(type='g-32vcpu-128gb')
    if not config.is_valid():
        config = AwsConfig(type='c4.8xlarge', region='us-east-2', image='ami-0b9064170e32bde34')

    if not config.is_valid():
        raise RuntimeError("No valid docker-machine config found!")

    # create new docker machine
    dm = DockerMachine(name='raytracer',
                       cwd='../',
                       config=config.config(),
                       user_env={
                           'SCENARIO': scenario,
                           'OUTPUT': 'raytraced_frame.jpeg',
                           'VOLUME': '/root/output/'
                       })

    dm.tskRunServices(timeout=60*60)
    dm.tskSecureCopyFromMachine("/root/output/raytraced_frame.jpeg", "raytraced.jpeg")
    dm.tskStopMachine()
    dm.tskKillMachine()
    dm.tskRemoveMachine()
    return dm


@click.command('runner')
@click.argument('scenario')
def runner(scenario):
    logging.basicConfig(level=20)
    logger = logging.getLogger(__name__)

    dm = start_render_machine(scenario)

    # wait for rendering to complete
    idle = False
    while dm.busy() or not idle:
        idle = True
        try:
            text = dm._stdout_queue.get(block=False)
            logger.info(text)
            idle = False

        except Exception:
            pass

        try:
            text = dm._stderr_queue.get(block=False)
            logger.error(text)
            idle = False

        except Exception:
            pass

        if idle:
            time.sleep(0.2)


if __name__ == "__main__":
    runner()
