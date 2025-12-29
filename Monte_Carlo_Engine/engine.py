import Core_Gameplay.gameplay as game
from Core_Gameplay.gameplay import PokerEvaluator
import numpy as np

class monte_carlo_engine:
    def __init__(self, table, hero_index=0):
        self.table = table
        self.hero = table.players[hero_index]
        self.n_opponents = len(table.players) - 1
        self.rng = np.random.default_rng()
    
        self.missing_board_count = 5 - len(table.community_cards)
        self.unseen_count = 52 - (2 * len(table.players)) - len(table.community_cards)
    
    def simulate(self, iterations=10000):
        # Setup
        full_deck_ints = set(game.Deck().cards)
        known_ints = set(self.hero.hand) | set(self.table.community_cards)

        # Your NumPy array of integers
        unseen_cards = np.array(list(full_deck_ints - known_ints))
        results = []
        wins = 0

        # Simulate over many scenarios
        for i in range(1, iterations+1):
            # Number of unseen cards possible for the villains
            num_villains = len(self.table.players) - 1
            cards_for_villains = num_villains * 2

            # Number of unseen cards left for community
            cards_for_board = 5 - len(self.table.community_cards)

            total_to_draw = cards_for_villains + cards_for_board

            shuffled_trial = self.rng.permutation(unseen_cards)
            community = list(self.table.community_cards)
            sim_cards = shuffled_trial[:total_to_draw]

            # Fill Possible Villains Cards
            all_villain_hands = []
            for v in range(num_villains):
                start = v * 2
                end = start + 2
                all_villain_hands.append(sim_cards[start:end])

            # Fill Community Cards    
            board_start = num_villains * 2
            sim_board_ext = sim_cards[board_start:]
            full_board = list(self.table.community_cards) + list(sim_board_ext)
            
            # Evaluate if Hero Wins
            has_won = True
            is_tie = False
            hero_eval, hero_best = PokerEvaluator.evaluate_player(self, full_board, self.hero.hand)

            for villain_hand in all_villain_hands:
                villain_eval, villain_best =  PokerEvaluator.evaluate_player(self, full_board, villain_hand)

                if villain_eval < hero_eval: # Villain has a better (lower) rank
                    has_won = False
                    is_tie = False
                    break # No need to check other villains, Hero lost this trial
                elif villain_eval == hero_eval:
                    is_tie = True

            if has_won and is_tie:
                wins += 0.5
            elif has_won:
                wins += 1
            
            if i % 100 == 0:
                current_rate = (wins / i) * 100
                results.append(current_rate)
        
        return (wins / iterations) * 100, results



