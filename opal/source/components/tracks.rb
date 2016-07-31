class Tracks
  include Inesita::Component

  def init
    puts "init Main"
    store.load_tracks
  end

  def play
    #store.play
  end

  def queue(track_id)
    store.play(track_id.to_s)
  end

  def tracks
    store.tracks.sort! do |x, y|
      x.play_count <=> y.play_count
    end
  end

  def render
    div id: 'track-list' do
      ul do
        li do
          span class: 'col1' do ; end
          span class: 'col2' do 'SONG'; end
          span class: 'col3' do 'ARTIST'; end
          span class: 'col4' do 'ALBUM'; end
          span class: 'col5' do 'PLAYS'; end
          span class: 'col6' do 'SKIPS'; end
        end
        tracks.reverse_each do |track|
          li onclick: -> { queue(track.id) } do
            span class: 'col1' do ; end
            span class: 'col2' do track.title; end
            span class: 'col3' do track.artists; end
            span class: 'col4' do track.album; end
            span class: 'col5' do track.play_count; end
            span class: 'col6' do track.skip_count; end
          end
        end
      end
    end
  end
end
