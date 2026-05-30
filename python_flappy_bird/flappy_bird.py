import random

import pygame

pygame.init()


class Config:
    COLOR_WHITE = (255, 255, 255)
    SCREEN_HEIGHT_ADJUST = 200
    SCREEN_LENGTH = 576
    SCREEN_HEIGHT = 1024 - SCREEN_HEIGHT_ADJUST
    FLOOR_Y = 900 - SCREEN_HEIGHT_ADJUST
    BACKGROUND_X = 0
    BACKGROUND_Y = -300
    FramePerSecond = 120
    BIRD_X = 200
    BIRD_Y = SCREEN_HEIGHT / 2
    BIRD_GRAVITY = 0.2
    BIRD_FLY_UP_SPEED = -6
    SCREEN = pygame.display.set_mode((SCREEN_LENGTH, SCREEN_HEIGHT))
    FONT = pygame.font.SysFont("Lucida Console", 40, bold=True)


class Score:
    def __init__(self):
        self.score = 0
        self.best_score = 0

    def show_score(self, pipe_pair_count):
        self.score = pipe_pair_count
        score_surface = Config.FONT.render(str(self.score), True, Config.COLOR_WHITE)
        score_rect = score_surface.get_rect(center=(288, 100))
        Config.SCREEN.blit(score_surface, score_rect)


class Pipe:
    bottom_pipe_surface = pygame.image.load("pipe.png").convert_alpha()
    bottom_pipe_surface = pygame.transform.scale(bottom_pipe_surface, (200, 1000))
    top_pipe_surface = pygame.transform.flip(bottom_pipe_surface, False, True)
    CREATE_PIPE_EVENT = pygame.USEREVENT
    bottom_pipe_y_choices = [400, 550, 700]

    def __init__(self, is_bottom, pipe_y):
        self.is_bottom = is_bottom

        if is_bottom:
            self.pipe_rect = Pipe.bottom_pipe_surface.get_rect(midtop=(1700, pipe_y))
        else:
            self.pipe_rect = Pipe.top_pipe_surface.get_rect(midbottom=(1700, pipe_y))

    def show(self):
        if self.is_bottom:
            Config.SCREEN.blit(Pipe.bottom_pipe_surface, self.pipe_rect)
        else:
            Config.SCREEN.blit(Pipe.top_pipe_surface, self.pipe_rect)
        self.pipe_rect.centerx -= 3

        if self.pipe_rect.centerx < 0:
            return 1
        else:
            return 0



    @staticmethod
    def create_a_pair_of_pipes() -> tuple:
        bottom_pipe_y = random.choice(Pipe.bottom_pipe_y_choices)
        top_pipe_y = bottom_pipe_y - 300
        bottom_pipe = Pipe(True, bottom_pipe_y)
        top_pipe = Pipe(False, top_pipe_y)
        return bottom_pipe, top_pipe

    @staticmethod
    def start_create_pipe_timer():
        pygame.time.set_timer(Pipe.CREATE_PIPE_EVENT, 1500)


class Bird:
    def __init__(self):
        self.bird_surface = pygame.image.load("Flappy_bird_player.png").convert_alpha()
        self.bird_surface = pygame.transform.scale(self.bird_surface, (100, 70))
        self.bird_rect = self.bird_surface.get_rect(center=(Config.BIRD_X, Config.BIRD_Y))

        self.bird_speed_y = 0

    def rotate(self):
        return pygame.transform.rotozoom(self.bird_surface, -self.bird_speed_y * 3, 1)

    def show(self):
        self.bird_speed_y += Config.BIRD_GRAVITY
        self.bird_rect.centery += self.bird_speed_y

        Config.SCREEN.blit(self.rotate(), self.bird_rect)

    def flip_wing(self):
        self.bird_speed_y = Config.BIRD_FLY_UP_SPEED

    def check_collision(self, pipe_list):
        for pipe in pipe_list:
            if self.bird_rect.colliderect(pipe.pipe_rect):
                print("Collision!")
                return True

        if self.bird_rect.bottom < 0:
            print("Fly too high!")
            return True

        if self.bird_rect.top > Config.FLOOR_Y:
            print("Fly too low!")
            return True

        return False

    def back_to_init(self):
        self.bird_speed_y = 0
        self.bird_rect.centery = Config.BIRD_Y


class Background:
    def __init__(self):
        self.background_surface = pygame.image.load("FlappyBird_background.png").convert()
        self.background_surface = pygame.transform.scale2x(self.background_surface)

    def show(self):
        Config.SCREEN.blit(self.background_surface, (Config.BACKGROUND_X, Config.BACKGROUND_Y))


class Floor:
    def __init__(self):
        self.floor_surface = pygame.image.load("floor.png").convert()
        self.floor_surface = pygame.transform.scale2x(self.floor_surface)
        self.floor_x = 0

    def show(self):
        Config.SCREEN.blit(self.floor_surface, (self.floor_x, Config.FLOOR_Y))
        self.floor_x -= 0.5
        if self.floor_x < -100:
            self.floor_x = 0


class FlappyBird:
    def __init__(self):
        # music = pygame.mixer.Sound("C418.mp3")
        self.background = Background()
        self.floor = Floor()
        self.clock = pygame.time.Clock()
        self.bird = Bird()
        self.pipes = []
        self.pipes.extend(Pipe.create_a_pair_of_pipes())
        self.game_on = True
        self.score = Score()
        # music.play()

    def back_to_init(self):
        self.pipes.clear()
        self.pipes.extend(Pipe.create_a_pair_of_pipes())
        self.bird.back_to_init()

    def handle_events(self):
        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                pygame.quit()
                exit()

            if event.type == pygame.KEYDOWN:

                if event.key == pygame.K_w:
                    if self.game_on:
                        self.bird.flip_wing()

                    else:
                        self.game_on = True
                        self.back_to_init()

            if event.type == Pipe.CREATE_PIPE_EVENT:
                self.pipes.extend(Pipe.create_a_pair_of_pipes())

    def begin(self):
        Pipe.start_create_pipe_timer()

        while True:
            self.handle_events()
            self.background.show()
            self.floor.show()
            pipe_count = 0
            if self.game_on:

                self.bird.show()
                for pipe in self.pipes:
                    pipe_count += pipe.show()

                collision_happened = self.bird.check_collision(self.pipes)
                if collision_happened:
                    self.game_on = False
                    print(f"Game on: {self.game_on}")

            self.score.show_score(pipe_count // 2)

            pygame.display.flip()
            self.clock.tick(Config.FramePerSecond)


if __name__ == "__main__":
    game = FlappyBird()
    game.begin()
